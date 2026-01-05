/**
 * @file mainwindow.cpp
 * @brief 主視窗類別實作檔案
 * 
 * 實作人臉辨識門禁系統的所有核心功能，包括影像處理、深度學習推論、
 * 資料庫操作與網路通訊。
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QRegularExpression>
#include <cstdlib>

namespace {
// Discord 通知冷卻時間 (毫秒)，避免短時間內重複送出訊息
constexpr int kNotificationCooldownMs = 3000;
}

/**
 * @brief MainWindow 建構子
 * 
 * 初始化整個門禁系統，包括：
 * 1. Discord 通知設定（從環境變數讀取）
 * 2. SQLite 資料庫初始化與資料表建立
 * 3. 深度學習模型載入（人臉偵測與特徵提取）
 * 4. 攝影機初始化
 * 5. 計時器設定（視訊更新與門禁控制）
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ==================== Discord 通知設定 ====================
    // 從環境變數讀取 Discord Bot Token 與 Channel ID
    // 若未提供則視為停用通知功能
    discordManager.reset(new QNetworkAccessManager(this));
    discordToken = qgetenv("DISCORD_TOKEN").trimmed();
    QByteArray channelEnv = qgetenv("CHANNEL_ID").trimmed();
    if (channelEnv.isEmpty())
        channelEnv = qgetenv("DISCORD_CHANNEL_ID").trimmed();
    discordChannelId = QString::fromUtf8(channelEnv).trimmed();
    discordMessageText = QString::fromUtf8(qgetenv("DISCORD_MESSAGE")).trimmed();
    if (discordMessageText.isEmpty()) {
        // 預設訊息內容
        discordMessageText = tr("有人來", "Discord notification when a recognized user is detected");
    }

    // 驗證 Channel ID 格式（必須為純數字）
    const QRegularExpression idPattern(QStringLiteral("^\\d+$"));
    if (!discordChannelId.isEmpty() && !idPattern.match(discordChannelId).hasMatch()) {
        qDebug() << "Discord notifier disabled: invalid CHANNEL_ID format";
        discordChannelId.clear();
    }

    // 驗證 Token 格式（不應包含空白字元）
    QString tokenString = QString::fromUtf8(discordToken);
    const QRegularExpression tokenWhitespace(QStringLiteral("\\s"));
    bool tokenValid = true;
    if (!tokenString.isEmpty() && tokenString.contains(tokenWhitespace)) {
        qDebug() << "Discord notifier disabled: token contains whitespace";
        tokenValid = false;
    }
    // 安全清除敏感資料
    tokenString.fill(u'0');
    tokenString.clear();
    if (!tokenValid) {
        discordToken.clear();
    }

    // 若缺少必要設定則停用 Discord 通知
    if (discordToken.isEmpty() || discordChannelId.isEmpty()) {
        qDebug() << "Discord notifier disabled: missing DISCORD_TOKEN or CHANNEL_ID";
    }

    // ==================== 資料庫初始化 ====================
    // 建立或開啟 SQLite 資料庫檔案
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");

    if (!db.open()) {
        qDebug() << "DB open failed:" << db.lastError().text();
    } else {
        qDebug() << "DB opened";
    }

    // 刪除舊的使用者資料表（重新開始）
    QSqlQuery q;
    if(!q.exec("DROP TABLE IF EXISTS users")) {
        qDebug() << "Failed to drop users table:" << q.lastError().text();
    }

    // 建立使用者資料表
    // 結構：id (主鍵), name (姓名), v1-v128 (128 維特徵向量)
    QString createTable = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT";
    for (int i = 1; i <= 128; ++i)
        createTable += QString(", v%1 REAL").arg(i);
    createTable += ");";

    if(!q.exec(createTable))
        qDebug() << "Create table failed:" << q.lastError().text();


    // 清空使用者快取
    usersCache.clear();

    // ==================== 深度學習模型載入 ====================
    // 取得模型檔案的基礎路徑
    QString basePath = QCoreApplication::applicationDirPath() + "/face/";

    // 載入 SSD 人臉偵測模型（Caffe 格式）
    // 輸入：300x300 BGR 影像
    // 輸出：人臉位置與信心度
    faceNet = cv::dnn::readNetFromCaffe(
        (basePath + "deploy.prototxt").toStdString(),
        (basePath + "res10_300x300_ssd_iter_140000.caffemodel").toStdString()
        );

    // 載入 OpenFace 人臉特徵提取模型（Torch 格式）
    // 輸入：96x96 RGB 人臉影像
    // 輸出：128 維特徵向量
    embedNet = cv::dnn::readNetFromTorch(
        (basePath + "openface_nn4.small2.v1.t7").toStdString()
        );

    // 檢查模型是否載入成功
    if (faceNet.empty() || embedNet.empty()) {
        qDebug() << "DNN model load FAILED";
    } else {
        qDebug() << "DNN models loaded OK";
    }

    // ==================== 攝影機初始化 ====================
    // 開啟預設攝影機（設備 ID 0）
    cap.open(0);
    if (!cap.isOpened()) {
        qDebug() << "Camera open failed";
        return;
    }

    // ==================== 計時器設定 ====================
    // 主計時器：每 60 毫秒觸發一次，更新視訊幀並進行人臉偵測
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(60);

    // 門禁計時器：單次觸發，用於自動關閉門禁
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
 * 
 * 釋放所有系統資源：
 * 1. 釋放攝影機資源
 * 2. 中止並清理所有活躍的網路請求
 * 3. 安全清除敏感資料（Discord Token）
 * 4. 刪除 UI 物件
 */
MainWindow::~MainWindow()
{
    cap.release();  // 釋放攝影機
    // 中止所有進行中的網路請求
    for (QNetworkReply *reply : activeReplies) {
        if (reply) {
            reply->abort();
            reply->deleteLater();
        }
    }
    activeReplies.clear();
    discordToken.fill(0);  // 覆寫敏感資料
    delete ui;
}

/**
 * @brief 更新視訊幀並執行人臉偵測與辨識
 * 
 * 此函數由主計時器定期觸發（每 60 毫秒），執行以下流程：
 * 1. 從攝影機擷取當前幀
 * 2. 使用 SSD 模型偵測所有人臉
 * 3. 對每個偵測到的人臉進行辨識
 * 4. 更新 UI 顯示授權狀態與門禁狀態
 * 5. 必要時傳送 Discord 通知
 * 6. 自動控制門禁開關
 */
void MainWindow::updateFrame()
{
    // 從攝影機擷取影像
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;  // 擷取失敗則跳過此幀

    // ==================== 人臉偵測 ====================
    // 將影像轉換為 blob 格式，並進行預處理
    // 尺寸：300x300，平均值減法：(104,177,123)
    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300,300),
                                          cv::Scalar(104,177,123), false, false);
    faceNet.setInput(blob);
    cv::Mat det = faceNet.forward();  // 執行人臉偵測推論
    cv::Mat detMat(det.size[2], det.size[3], CV_32F, det.ptr<float>());

    // 初始化授權狀態
    bool authorized = false;
    int userId = -1;

    // ==================== 處理偵測結果 ====================
    // 遍歷所有偵測到的人臉
    for (int i = 0; i < detMat.rows; i++) {
        // 取得信心度
        float conf = detMat.at<float>(i, 2);
        if (conf < 0.6) continue;  // 過濾信心度過低的偵測結果

        // 計算人臉邊界框座標（相對於原始影像）
        int x1 = int(detMat.at<float>(i, 3) * frame.cols);
        int y1 = int(detMat.at<float>(i, 4) * frame.rows);
        int x2 = int(detMat.at<float>(i, 5) * frame.cols);
        int y2 = int(detMat.at<float>(i, 6) * frame.rows);

        // 在影像上繪製綠色矩形框標示人臉位置
        cv::Rect faceRect(cv::Point(x1,y1), cv::Point(x2,y2));
        cv::rectangle(frame, faceRect, cv::Scalar(0,255,0), 2);

        // ==================== 人臉辨識 ====================
        // 提取人臉感興趣區域並進行預處理
        cv::Mat faceROI = frame(faceRect).clone();
        cv::cvtColor(faceROI, faceROI, cv::COLOR_BGR2RGB);  // BGR 轉 RGB
        cv::resize(faceROI, faceROI, cv::Size(96,96));      // 調整為 96x96

        // 執行人臉辨識
        if (recognizeFace(faceROI, userId)) {
            authorized = true;  // 辨識成功，授權通行
        }
    }

    // ==================== 更新 UI 與門禁控制 ====================
    if(authorized){
        // 顯示授權狀態與使用者 ID
        ui->label_status->setText("Authorized\nID: " + QString::number(userId));
        ui->label_status->setStyleSheet("color:green; font-weight:bold;");
        
        // 傳送 Discord 通知（若符合條件）
        if(canSendNotification()){
            QString message = discordMessageText;
            if(userId >= 0){
                message += QString(" (ID: %1)").arg(userId);
            }
            sendDiscordMessage(message);
            notificationCooldown.restart();  // 重新啟動冷卻計時器
            notificationSent = true;
        }
        
        // 開啟門禁 3 秒鐘
        if(!doorOpen){
            doorOpen = true;
            doorTimer->start(3000);  // 3 秒後自動關門
        }
    } else {
        // 未授權：顯示鎖定狀態
        notificationSent = false;
        ui->label_status->setText("Door Locked");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
    }

    // ==================== 顯示影像於 UI ====================
    // 將 BGR 格式轉換為 RGB（Qt 使用 RGB 格式）
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    // 建立 QImage 並顯示於 label_camera 控件
    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    ui->label_camera->setPixmap(QPixmap::fromImage(img)
                                    .scaled(ui->label_camera->size(), Qt::KeepAspectRatio));
}

/**
 * @brief 註冊按鈕點擊事件處理函數
 * 
 * 執行使用者註冊流程：
 * 1. 驗證輸入的姓名
 * 2. 擷取當前攝影機畫面
 * 3. 偵測人臉並選擇信心度最高者
 * 4. 提取人臉特徵向量
 * 5. 儲存至資料庫
 */
void MainWindow::on_pushButton_register_clicked()
{
    // 取得並驗證使用者輸入的姓名
    QString name = ui->lineEdit_name->text().trimmed();
    if(name.isEmpty()){
        ui->label_status->setText("Name cannot be empty");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // 從攝影機擷取影像
    cv::Mat frame;
    cap >> frame;
    if(frame.empty()){
        ui->label_status->setText("Camera capture failed");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // ==================== 人臉偵測 ====================
    // 使用 SSD 模型偵測影像中的所有人臉
    cv::Mat blob = cv::dnn::blobFromImage(frame,1.0,cv::Size(300,300),cv::Scalar(104,177,123),false,false);
    faceNet.setInput(blob);
    cv::Mat det = faceNet.forward();
    cv::Mat detMat(det.size[2], det.size[3], CV_32F, det.ptr<float>());

    // 尋找信心度最高的人臉
    int bestIdx=-1;
    float maxConf=0;
    for(int i=0;i<detMat.rows;i++){
        float conf = detMat.at<float>(i,2);
        if(conf > maxConf){
            maxConf = conf;
            bestIdx = i;
        }
    }

    // 檢查是否成功偵測到人臉
    if(bestIdx==-1 || maxConf<0.6){
        ui->label_status->setText("No face detected");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // ==================== 提取人臉區域 ====================
    // 計算人臉邊界框座標
    int x1=int(detMat.at<float>(bestIdx,3)*frame.cols);
    int y1=int(detMat.at<float>(bestIdx,4)*frame.rows);
    int x2=int(detMat.at<float>(bestIdx,5)*frame.cols);
    int y2=int(detMat.at<float>(bestIdx,6)*frame.rows);
    cv::Rect faceRect(cv::Point(x1,y1), cv::Point(x2,y2));

    // 提取人臉 ROI 並進行預處理
    cv::Mat faceROI = frame(faceRect).clone();
    cv::cvtColor(faceROI, faceROI, cv::COLOR_BGR2RGB);  // BGR 轉 RGB
    cv::resize(faceROI, faceROI, cv::Size(96,96));      // 調整為 96x96

    // ==================== 特徵提取 ====================
    // 使用 OpenFace 模型提取 128 維特徵向量
    cv::Mat blobFace = cv::dnn::blobFromImage(faceROI,1.0/255.0,cv::Size(96,96),cv::Scalar(0,0,0),true,false);
    embedNet.setInput(blobFace);
    cv::Mat vec = embedNet.forward();  // 輸出 128 維特徵向量

    // ==================== 儲存至資料庫 ====================
    // 將姓名與特徵向量儲存至 SQLite 資料庫
    if(addFaceToDB(name, vec)){
        ui->label_status->setText("Registered: " + name);
        ui->label_status->setStyleSheet("color:green; font-weight:bold;");
    } else {
        ui->label_status->setText("Register failed");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
    }
}

/**
 * @brief 刪除按鈕點擊事件處理函數
 * 
 * 根據使用者輸入的姓名，從資料庫中刪除對應的使用者資料。
 */
void MainWindow::on_pushButton_DELETE_clicked()
{
    // 取得並驗證使用者輸入的姓名
    QString name = ui->lineEdit_DELETNAME->text().trimmed();
    if(name.isEmpty()){
        ui->label_status->setText("Name cannot be empty");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // 執行刪除 SQL 查詢
    QSqlQuery q;
    q.prepare("DELETE FROM users WHERE name = ?");
    q.addBindValue(name);

    if(!q.exec()){
        qDebug() << "Delete failed:" << q.lastError().text();
        ui->label_status->setText("Delete failed");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    // 檢查刪除結果並更新 UI
    if(q.numRowsAffected() == 0){
        // 未找到指定姓名的使用者
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
 * @brief 將人臉特徵向量新增至資料庫
 * @param name 使用者姓名
 * @param vec 128 維人臉特徵向量（OpenFace 模型輸出）
 * @return 成功返回 true，失敗返回 false
 * 
 * 將特徵向量的 128 個浮點數值分別儲存至資料表的 v1-v128 欄位。
 */
bool MainWindow::addFaceToDB(const QString &name, const cv::Mat &vec)
{
    // ==================== 向量格式轉換 ====================
    // 確保向量為 1x128 的橫向格式
    cv::Mat vecF;
    if(vec.rows == 128 && vec.cols == 1)
        vecF = vec.t();  // 轉置為 1x128
    else
        vecF = vec.clone();

    vecF.convertTo(vecF, CV_32F);  // 轉換為 32 位元浮點數

    // 驗證向量尺寸
    if(vecF.rows != 1 || vecF.cols != 128){
        qDebug() << "Vector size invalid:" << vecF.rows << "x" << vecF.cols;
        return false;
    }

    // ==================== 建立 SQL 插入語句 ====================
    // 動態建立包含 128 個欄位的 INSERT 語句
    QString cmd = "INSERT INTO users (name";
    QString vals = " VALUES (?";
    for(int i=1; i<=128; i++){
        cmd += QString(", v%1").arg(i);
        vals += ", ?";
    }
    cmd += ")";
    vals += ")";
    cmd += vals;

    // ==================== 執行資料庫插入 ====================
    QSqlQuery q;
    q.prepare(cmd);
    q.addBindValue(name);  // 綁定姓名
    // 綁定 128 維特徵向量
    for(int i=0; i<128; i++){
        q.addBindValue(vecF.at<float>(0,i));
    }

    if(!q.exec()){
        qDebug() << "Insert failed:" << q.lastError().text();
        return false;
    }
    return true;
}

/**
 * @brief 辨識人臉
 * @param faceROI 人臉感興趣區域（96x96 RGB 影像）
 * @param outId 輸出參數，儲存辨識到的使用者 ID
 * @return 辨識成功返回 true，失敗返回 false
 * 
 * 使用歐氏距離比對特徵向量：
 * 1. 提取輸入人臉的特徵向量
 * 2. 與資料庫中所有使用者的特徵向量計算距離
 * 3. 距離小於 0.8 視為同一人（閾值可調整）
 */
bool MainWindow::recognizeFace(const cv::Mat &faceROI, int &outId)
{
    // ==================== 特徵提取 ====================
    // 將人臉影像轉換為 blob 並正規化
    cv::Mat blob = cv::dnn::blobFromImage(faceROI, 1.0/255.0, cv::Size(96,96),
                                          cv::Scalar(0,0,0), true, false);
    embedNet.setInput(blob);
    cv::Mat vec = embedNet.forward(); // 輸出 1x128 特徵向量

    // ==================== 從資料庫讀取所有使用者 ====================
    QString selectSql = "SELECT id";
    for(int i=1;i<=128;i++) selectSql += QString(", v%1").arg(i);
    selectSql += " FROM users";

    QSqlQuery q(selectSql);

    // ==================== 比對特徵向量 ====================
    // 遍歷資料庫中的每個使用者
    while(q.next())
    {
        int id = q.value(0).toInt();
        // 重建使用者的特徵向量
        cv::Mat dbVec(1,128,CV_32F);
        for(int i=0;i<128;i++){
            dbVec.at<float>(0,i) = q.value(i+1).toFloat();
        }

        // 計算歐氏距離
        float dist = cv::norm(vec - dbVec);
        if(dist < 0.8){  // 距離閾值：0.8
            qDebug() << "Recognized ID:" << id << "Distance:" << dist;
            outId = id;
            return true;  // 辨識成功
        }
    }

    return false;  // 未找到匹配的人臉
}

/**
 * @brief 傳送 Discord 訊息通知
 * @param text 訊息內容
 * 
 * 透過 Discord Bot API 將訊息傳送至指定頻道。
 * 使用非同步方式傳送，不會阻塞主執行緒。
 */
void MainWindow::sendDiscordMessage(const QString &text)
{
    // 檢查 Discord 設定是否完整
    if (discordToken.isEmpty() || discordChannelId.isEmpty() || !discordManager)
        return;

    // ==================== 建立 HTTP 請求 ====================
    // 建立 Discord API URL
    QUrl url(QStringLiteral("https://discord.com"));
    url.setPath(QStringLiteral("/api/v10/channels/%1/messages").arg(discordChannelId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    // 設定 Bot 授權標頭
    QByteArray authHeader("Bot ");
    authHeader.append(discordToken);
    request.setRawHeader("Authorization", authHeader);

    // 建立 JSON 訊息內容
    QJsonObject body;
    body["content"] = text;

    // ==================== 傳送 POST 請求 ====================
    QNetworkReply *reply = discordManager->post(request, QJsonDocument(body).toJson());
    if (!reply) {
        qDebug() << "Discord send failed: no reply";
        return;
    }
    // 追蹤活躍的網路請求
    activeReplies.insert(reply);
    // 設定完成回呼：處理錯誤並清理資源
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        activeReplies.remove(reply);
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Discord send failed:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

/**
 * @brief 檢查是否可以傳送 Discord 通知
 * @return 可以傳送返回 true，否則返回 false
 * 
 * 確認以下條件：
 * 1. Discord 設定完整（Token 與 Channel ID 存在）
 * 2. 冷卻時間已過（避免重複通知）
 * 3. 當前狀態未傳送通知
 */
bool MainWindow::canSendNotification() const
{
    const bool hasConfig = !discordToken.isEmpty() && !discordChannelId.isEmpty();
    const bool cooldownReady = !notificationCooldown.isValid() || notificationCooldown.elapsed() >= kNotificationCooldownMs;
    return hasConfig && cooldownReady && !notificationSent;
}
