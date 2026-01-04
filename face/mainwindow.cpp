/**
 * @file mainwindow.cpp
 * @brief 主視窗類別實作檔
 * @description 提供無 OpenCV 的簡化門禁介面與資料庫操作
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

/**
 * @brief MainWindow 建構子
 * @param parent 父視窗指標
 * @description 初始化資料庫並啟動狀態更新定時器（不使用攝影機或深度學習模型）
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");

    if (!db.open()) {
        qDebug() << "DB open failed:" << db.lastError().text();
    } else {
        qDebug() << "DB opened";
    }

    QSqlQuery q;
    if(!q.exec("DROP TABLE IF EXISTS users")) {
        qDebug() << "Failed to drop users table:" << q.lastError().text();
    }

    if(!q.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)"))
        qDebug() << "Create table failed:" << q.lastError().text();

    ui->label_camera->setText("Camera preview unavailable (OpenCV removed)");
    ui->label_camera->setAlignment(Qt::AlignCenter);
    ui->label_status->setText("Face recognition disabled (OpenCV removed)");
    ui->label_status->setStyleSheet("color:orange; font-weight:bold;");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(1000);
}

/**
 * @brief MainWindow 解構子
 * @description 釋放攝影機資源和 UI 物件
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief 更新狀態顯示
 * @description 定時器觸發此函數，更新提示文字與時間戳
 */
void MainWindow::updateFrame()
{
    ui->label_status->setText(
        QString("Face recognition disabled (OpenCV removed)\n%1")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
}

/**
 * @brief 註冊按鈕點擊事件處理
 * @description 當使用者點擊註冊按鈕時，將輸入姓名寫入 SQLite 名單
 */
void MainWindow::on_pushButton_register_clicked()
{
    QString name = ui->lineEdit_name->text().trimmed();
    if(name.isEmpty()){
        ui->label_status->setText("Name cannot be empty");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO users (name) VALUES (?)");
    q.addBindValue(name);

    if(q.exec()){
        ui->label_status->setText("Registered: " + name);
        ui->label_status->setStyleSheet("color:green; font-weight:bold;");
    } else {
        ui->label_status->setText("Register failed");
        ui->label_status->setStyleSheet("color:red; font-weight:bold;");
        qDebug() << "Insert failed:" << q.lastError().text();
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
