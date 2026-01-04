/**
 * @file mainwindow.cpp
 * @brief 主視窗類別實作檔
 * @description 實作人臉辨識門禁系統的核心功能
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

/**
 * @brief MainWindow 建構子
 * @param parent 父視窗指標
 * @description 初始化資料庫、載入深度學習模型、開啟攝影機並啟動定時器
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // 設定 UI 介面
    ui->setupUi(this);

    // === 資料庫初始化 ===
    // 建立 SQLite 資料庫連線
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");

    // 開啟資料庫
    if (!db.open()) {
        qDebug() << "DB open failed:" << db.lastError().text();
    } else {
        qDebug() << "DB opened";
    }

    // 重置資料表 (開發階段使用，每次啟動都會清空)
    QSqlQuery q;
    if(!q.exec("DROP TABLE IF EXISTS users")) {
        qDebug() << "Failed to drop users table:" << q.lastError().text();
    }

    // 建立使用者資料表
    // 結構: id (自動遞增), name (文字), v1~v128 (人臉特徵向量的 128 個維度)
    QString createTable = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT";
    for (int i = 1; i <= 128; ++i)
        createTable += QString(", v%1 REAL").arg(i);
    createTable += ");";

    if(!q.exec(createTable))
        qDebug() << "Create table failed:" << q.lastError().text();

    // 清空使用者快取
    usersCache.clear();

    // === 深度學習模型載入 ===
    // 取得模型檔案路徑 (假設在應用程式目錄下的 face/ 資料夾)
    QString basePath = QCoreApplication::applicationDirPath() + "/face/";

    // 載入人臉偵測模型 (Caffe SSD)
    // deploy.prototxt: 網路架構定義
    // res10_300x300_ssd_iter_140000.caffemodel: 訓練好的權重
    faceNet = cv::dnn::readNetFromCaffe(
        (basePath + "deploy.prototxt").toStdString(),
        (basePath + "res10_300x300_ssd_iter_140000.caffemodel").toStdString()
        );

    // 載入人臉特徵提取模型 (OpenFace)
    // 將人臉影像轉換為 128 維特徵向量
    embedNet = cv::dnn::readNetFromTorch(
        (basePath + "openface_nn4.small2.v1.t7").toStdString()
        );

    // 檢查模型是否載入成功
    if (faceNet.empty() || embedNet.empty()) {
        qDebug() << "DNN model load FAILED";
    } else {
        qDebug() << "DNN models loaded OK";
    }

    // === 攝影機初始化 ===
    // 開啟預設攝影機 (索引 0)
    cap.open(0);
    if (!cap.isOpened()) {
        qDebug() << "Camera open failed";
        return;
    }

    // === 定時器設定 ===
    // 建立影像更新定時器，每 60 毫秒更新一次 (約 16 FPS)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(60);

    // 建立門禁控制定時器 (單次觸發)
    // 當辨識成功後，門會開啟 3 秒後自動關閉
    doorTimer = new QTimer(this);
    doorTimer->setSingleShot(true);
    connect(doorTimer, &QTimer::timeout, this, [=]() {
        doorOpen = false;
        ui->label_status->setText("Door Locked");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
    });
}

/**
 * @brief MainWindow 解構子
 * @description 釋放攝影機資源和 UI 物件
 */
MainWindow::~MainWindow()
{
    // 釋放攝影機
    cap.release();
    // 刪除 UI 介面
    delete ui;
}

/**
 * @brief 更新影像幀並進行人臉偵測與辨識
 * @description 定時器觸發此函數，從攝影機讀取影像，使用深度學習模型偵測人臉，
 *              並與資料庫中的人臉特徵進行比對，更新門禁狀態
 */
void MainWindow::updateFrame()
{
    // === 讀取攝影機影像 ===
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;  // 影像為空則跳過

    // === 人臉偵測 ===
    // 建立輸入 blob (Binary Large Object)
    // 參數: 影像, 縮放因子, 輸入尺寸, 均值減法 (BGR 順序), 不交換 R 和 B, 不裁切
    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300,300),
                                          cv::Scalar(104,177,123), false, false);
    
    // 設定網路輸入並執行前向傳播
    faceNet.setInput(blob);
    cv::Mat det = faceNet.forward();
    
    // 將偵測結果轉換為 2D 矩陣 (每列代表一個偵測結果)
    cv::Mat detMat(det.size[2], det.size[3], CV_32F, det.ptr<float>());

    // 辨識狀態旗標
    bool authorized = false;
    int userId = -1;

    // === 處理每個偵測到的人臉 ===
    for (int i = 0; i < detMat.rows; i++) {
        // 取得信心值 (第 3 列)
        float conf = detMat.at<float>(i, 2);
        if (conf < 0.6) continue;  // 信心值低於 0.6 則忽略

        // 取得邊界框座標 (已正規化為 0~1)
        int x1 = int(detMat.at<float>(i, 3) * frame.cols);
        int y1 = int(detMat.at<float>(i, 4) * frame.rows);
        int x2 = int(detMat.at<float>(i, 5) * frame.cols);
        int y2 = int(detMat.at<float>(i, 6) * frame.rows);

        // 建立人臉矩形區域
        cv::Rect faceRect(cv::Point(x1,y1), cv::Point(x2,y2));
        // 在原始影像上繪製綠色矩形框
        cv::rectangle(frame, faceRect, cv::Scalar(0,255,0), 2);

        // 裁切人臉區域並進行預處理
        cv::Mat faceROI = frame(faceRect).clone();
        cv::cvtColor(faceROI, faceROI, cv::COLOR_BGR2RGB);  // 轉換為 RGB
        cv::resize(faceROI, faceROI, cv::Size(96,96));      // 調整為 96x96

        // === 人臉辨識 ===
        if (recognizeFace(faceROI, userId)) {
            authorized = true;  // 辨識成功
        }
    }

    // === 更新 UI 狀態 ===
    if(authorized){
        // 辨識成功，顯示授權訊息
        ui->label_status->setText("Authorized\nID: " + QString::number(userId));
        ui->label_status->setStyleSheet("color:green; font-weight:bold;");
        
        // 開啟門禁並啟動 3 秒計時器
        if(!doorOpen){
            doorOpen = true;
            doorTimer->start(3000);  // 3000 毫秒後自動關閉
        }
    } else {
        // 未辨識到授權人臉，顯示鎖定訊息
        ui->label_status->setText("Door Locked");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
    }

    // === 顯示影像 ===
    // 將影像從 BGR 轉換為 RGB (Qt 使用 RGB 格式)
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    
    // 建立 QImage 並轉換為 QPixmap 顯示在 UI 上
    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    ui->label_camera->setPixmap(QPixmap::fromImage(img)
                                    .scaled(ui->label_camera->size(), Qt::KeepAspectRatio));
}

/**
 * @brief 註冊按鈕點擊事件處理
 * @description 當使用者點擊註冊按鈕時，捕捉當前攝影機影像中信心值最高的人臉，
 *              提取特徵向量並存入資料庫
 */
void MainWindow::on_pushButton_register_clicked()
{
    // === 檢查姓名輸入 ===
    QString name = ui->lineEdit_name->text().trimmed();
    if(name.isEmpty()){
        ui->label_status->setText("Name cannot be empty");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // === 捕捉影像 ===
    cv::Mat frame;
    cap >> frame;
    if(frame.empty()){
        ui->label_status->setText("Camera capture failed");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // === 人臉偵測 ===
    // 建立輸入 blob 並進行人臉偵測
    cv::Mat blob = cv::dnn::blobFromImage(frame,1.0,cv::Size(300,300),cv::Scalar(104,177,123),false,false);
    faceNet.setInput(blob);
    cv::Mat det = faceNet.forward();
    cv::Mat detMat(det.size[2], det.size[3], CV_32F, det.ptr<float>());

    // === 尋找信心值最高的人臉 ===
    int bestIdx=-1;
    float maxConf=0;
    for(int i=0;i<detMat.rows;i++){
        float conf = detMat.at<float>(i,2);
        if(conf > maxConf){
            maxConf = conf;
            bestIdx = i;
        }
    }

    // 檢查是否偵測到人臉且信心值足夠
    if(bestIdx==-1 || maxConf<0.6){
        ui->label_status->setText("No face detected");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // === 取得人臉區域 ===
    // 計算邊界框座標
    int x1=int(detMat.at<float>(bestIdx,3)*frame.cols);
    int y1=int(detMat.at<float>(bestIdx,4)*frame.rows);
    int x2=int(detMat.at<float>(bestIdx,5)*frame.cols);
    int y2=int(detMat.at<float>(bestIdx,6)*frame.rows);
    cv::Rect faceRect(cv::Point(x1,y1), cv::Point(x2,y2));

    // 裁切並預處理人臉影像
    cv::Mat faceROI = frame(faceRect).clone();
    cv::cvtColor(faceROI, faceROI, cv::COLOR_BGR2RGB);  // 轉換為 RGB
    cv::resize(faceROI, faceROI, cv::Size(96,96));      // 調整為 96x96

    // === 提取人臉特徵向量 ===
    // 建立正規化的輸入 blob (像素值除以 255)
    cv::Mat blobFace = cv::dnn::blobFromImage(faceROI,1.0/255.0,cv::Size(96,96),cv::Scalar(0,0,0),true,false);
    embedNet.setInput(blobFace);
    cv::Mat vec = embedNet.forward();  // 輸出 128 維特徵向量

    // === 儲存到資料庫 ===
    if(addFaceToDB(name, vec)){
        ui->label_status->setText("Registered: " + name);
        ui->label_status->setStyleSheet("color:green; font-weight:bold;");
    } else {
        ui->label_status->setText("Register failed");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
    }
}

/**
 * @brief 刪除按鈕點擊事件處理
 * @description 從資料庫中刪除指定姓名的使用者資料
 */
void MainWindow::on_pushButton_DELETE_clicked()
{
    // === 檢查姓名輸入 ===
    QString name = ui->lineEdit_DELETNAME->text().trimmed();
    if(name.isEmpty()){
        ui->label_status->setText("Name cannot be empty");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // === 執行刪除操作 ===
    // 建立參數化查詢以防止 SQL 注入
    QSqlQuery q;
    q.prepare("DELETE FROM users WHERE name = ?");
    q.addBindValue(name);

    // 執行刪除並檢查結果
    if(!q.exec()){
        qDebug() << "Delete failed:" << q.lastError().text();
        ui->label_status->setText("Delete failed");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // 檢查是否有資料被刪除
    if(q.numRowsAffected() == 0){
        // 找不到指定的使用者
        ui->label_status->setText("User not found: " + name);
        ui->label_status->setStyleSheet("color:orange; font-weight:bold;");
    } else {
        // 刪除成功
        ui->label_status->setText("Deleted: " + name);
        ui->label_status->setStyleSheet("color:green; font-weight:bold;");
    }

    qDebug() << "Deleted user:" << name;
}

/**
 * @brief 新增人臉資料到資料庫
 * @param name 使用者姓名
 * @param vec 人臉特徵向量 (128 維)
 * @return 成功返回 true，失敗返回 false
 * @description 將 128 維的人臉特徵向量展開並儲存到資料庫的 128 個欄位中
 */
bool MainWindow::addFaceToDB(const QString &name, const cv::Mat &vec)
{
    // === 向量格式轉換 ===
    cv::Mat vecF;
    // 檢查向量維度，必要時進行轉置
    if(vec.rows == 128 && vec.cols == 1)
        vecF = vec.t();  // 128x1 轉置為 1x128
    else
        vecF = vec.clone();

    // 確保資料類型為 32 位元浮點數
    vecF.convertTo(vecF, CV_32F);

    // 驗證向量維度是否正確
    if(vecF.rows != 1 || vecF.cols != 128){
        qDebug() << "Vector size invalid:" << vecF.rows << "x" << vecF.cols;
        return false;
    }

    // === 建立 SQL 插入語句 ===
    // 動態產生包含 128 個特徵值的 INSERT 語句
    QString cmd = "INSERT INTO users (name";
    QString vals = " VALUES (?";
    for(int i=1; i<=128; i++){
        cmd += QString(", v%1").arg(i);  // 欄位名稱: v1, v2, ..., v128
        vals += ", ?";                   // 參數佔位符
    }
    cmd += ")";
    vals += ")";
    cmd += vals;

    // === 執行資料庫插入 ===
    QSqlQuery q;
    q.prepare(cmd);
    q.addBindValue(name);  // 綁定姓名
    // 綁定 128 個特徵值
    for(int i=0; i<128; i++){
        q.addBindValue(vecF.at<float>(0,i));
    }

    // 執行查詢並檢查結果
    if(!q.exec()){
        qDebug() << "Insert failed:" << q.lastError().text();
        return false;
    }
    return true;
}

/**
 * @brief 辨識人臉
 * @param faceROI 人臉影像區域 (96x96 RGB)
 * @param outId 輸出參數，辨識成功時返回使用者 ID
 * @return 辨識成功返回 true，失敗返回 false
 * @description 將輸入的人臉影像轉換為 128 維特徵向量，
 *              與資料庫中所有使用者的特徵向量計算歐式距離，
 *              若距離小於閾值 (0.8) 則視為同一人
 */
bool MainWindow::recognizeFace(const cv::Mat &faceROI, int &outId)
{
    // === 提取人臉特徵向量 ===
    // 建立正規化的輸入 blob (像素值除以 255)
    cv::Mat blob = cv::dnn::blobFromImage(faceROI, 1.0/255.0, cv::Size(96,96),
                                          cv::Scalar(0,0,0), true, false);
    embedNet.setInput(blob);
    cv::Mat vec = embedNet.forward(); // 輸出 1x128 特徵向量

    // === 建立查詢語句 ===
    // 查詢所有使用者的 ID 和 128 維特徵向量
    QString selectSql = "SELECT id";
    for(int i=1;i<=128;i++) selectSql += QString(", v%1").arg(i);
    selectSql += " FROM users";

    QSqlQuery q(selectSql);

    // === 遍歷資料庫中的所有使用者 ===
    while(q.next())
    {
        // 取得使用者 ID
        int id = q.value(0).toInt();
        
        // 重建 128 維特徵向量
        cv::Mat dbVec(1,128,CV_32F);
        for(int i=0;i<128;i++){
            dbVec.at<float>(0,i) = q.value(i+1).toFloat();
        }

        // === 計算歐式距離 ===
        // 距離越小表示越相似
        float dist = cv::norm(vec - dbVec);
        
        // 判斷是否為同一人 (閾值 0.8)
        if(dist < 0.8){
            qDebug() << "Recognized ID:" << id << "Distance:" << dist;
            outId = id;
            return true;  // 辨識成功
        }
    }

    // 沒有找到匹配的人臉
    return false;
}
