/**
 * @file mainwindow.h
 * @brief 主視窗類別定義檔案
 * 
 * 此檔案定義了 MainWindow 類別，包含人臉辨識門禁系統的所有核心功能。
 * 整合了 Qt GUI、OpenCV 電腦視覺、深度學習模型、SQLite 資料庫與 Discord 通知。
 */

#pragma once

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTimer>
#include <QList>
#include <QString>
#include <QScopedPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSet>
#include <QByteArray>
#include <QElapsedTimer>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @struct User
 * @brief 使用者資料結構
 * 
 * 儲存單一使用者的完整資訊，包含資料庫 ID、姓名與 128 維人臉特徵向量。
 */
struct User {
    int id;          ///< 資料庫中的使用者唯一識別碼
    QString name;    ///< 使用者姓名
    cv::Mat vec;     ///< 128 維人臉特徵向量（OpenFace 模型輸出）
};

/**
 * @class MainWindow
 * @brief 主視窗類別
 * 
 * 負責整個人臉辨識門禁系統的邏輯實作，包括：
 * - 即時影像擷取與顯示
 * - 人臉偵測與辨識
 * - 使用者註冊與刪除
 * - 門禁控制
 * - Discord 通知整合
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 建構子
     * @param parent 父視窗指標，預設為 nullptr
     * 
     * 初始化所有系統元件，包括資料庫、深度學習模型、攝影機與計時器。
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 解構子
     * 
     * 釋放所有資源，包括攝影機、網路連線與敏感資料。
     */
    ~MainWindow();

private slots:
    /**
     * @brief 更新視訊幀並進行人臉偵測與辨識
     * 
     * 此函數由計時器定期觸發（每 60 毫秒），執行以下步驟：
     * 1. 從攝影機擷取影像
     * 2. 使用 SSD 模型偵測人臉
     * 3. 對偵測到的人臉進行辨識
     * 4. 更新 UI 顯示門禁狀態
     * 5. 必要時傳送 Discord 通知
     */
    void updateFrame();
    
    /**
     * @brief 註冊按鈕點擊事件處理
     * 
     * 擷取當前攝影機畫面中的人臉，提取特徵向量並儲存至資料庫。
     */
    void on_pushButton_register_clicked();
    
    /**
     * @brief 刪除按鈕點擊事件處理
     * 
     * 根據使用者名稱從資料庫中刪除對應的使用者資料。
     */
    void on_pushButton_DELETE_clicked();

private:
    Ui::MainWindow *ui;  ///< Qt Designer 生成的 UI 介面指標

    // 資料庫相關
    QSqlDatabase db;  ///< SQLite 資料庫連線，儲存使用者資料

    // 深度學習模型
    cv::dnn::Net faceNet;   ///< SSD 人臉偵測網路（Caffe 模型）
    cv::dnn::Net embedNet;  ///< OpenFace 人臉特徵提取網路（Torch 模型）

    // 影像擷取
    cv::VideoCapture cap;  ///< OpenCV 攝影機擷取物件

    // 計時器
    QTimer *timer;      ///< 主計時器，用於定期更新視訊幀
    QTimer *doorTimer;  ///< 門禁計時器，用於自動關門

    // 門禁狀態
    bool doorOpen = false;  ///< 門禁開啟狀態旗標

    // 使用者快取
    QList<User> usersCache;  ///< 使用者資料快取（目前未使用）

    /**
     * @brief 將人臉特徵向量新增至資料庫
     * @param name 使用者姓名
     * @param vec 128 維人臉特徵向量
     * @return 成功返回 true，失敗返回 false
     * 
     * 將人臉特徵向量的 128 個浮點數值儲存至資料庫的對應欄位（v1-v128）。
     */
    bool addFaceToDB(const QString &name, const cv::Mat &vec);
    
    /**
     * @brief 辨識人臉
     * @param faceROI 人臉感興趣區域（96x96 RGB 影像）
     * @param outId 輸出參數，儲存辨識到的使用者 ID
     * @return 辨識成功返回 true，失敗返回 false
     * 
     * 計算輸入人臉與資料庫中所有人臉的歐氏距離，距離小於 0.8 視為同一人。
     */
    bool recognizeFace(const cv::Mat &faceROI, int &outId);
    
    /**
     * @brief 刪除使用者
     * @param name 使用者姓名
     * @return 成功返回 true，失敗返回 false
     * 
     * 從資料庫中刪除指定姓名的使用者資料（目前未被直接使用）。
     */
    bool deleteUser(const QString &name);
    
    /**
     * @brief 傳送 Discord 訊息通知
     * @param text 訊息內容
     * 
     * 使用 Discord Bot API 傳送訊息至指定頻道，通知有人通過門禁。
     */
    void sendDiscordMessage(const QString &text);
    
    /**
     * @brief 檢查是否可以傳送 Discord 通知
     * @return 可以傳送返回 true，否則返回 false
     * 
     * 確認 Discord 設定有效且冷卻時間已過，避免短時間內重複通知。
     */
    bool canSendNotification() const;

    // Discord 通知相關成員
    QScopedPointer<QNetworkAccessManager> discordManager;  ///< 網路請求管理器
    QByteArray discordToken;                               ///< Discord Bot Token（敏感資料）
    QString discordChannelId;                              ///< Discord 頻道 ID
    QString discordMessageText;                            ///< Discord 通知訊息內容
    bool notificationSent = false;                         ///< 通知已傳送旗標
    QElapsedTimer notificationCooldown;                    ///< Discord 通知冷卻計時器
    QSet<QNetworkReply*> activeReplies;                    ///< 活躍的網路請求集合
};
