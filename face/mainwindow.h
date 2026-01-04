/**
 * @file mainwindow.h
 * @brief 主視窗類別標頭檔
 * @description 定義移除 OpenCV 後的簡化門禁介面
 */

#pragma once

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief 主視窗類別
 * @description 提供簡單的名單新增/刪除與狀態顯示（不含人臉辨識）
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 建構子
     * @param parent 父視窗指標，預設為 nullptr
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
     /**
      * @brief 解構子
      * @description 釋放 UI 資源
      */
    ~MainWindow();

private slots:
    /**
     * @brief 更新狀態顯示
     * @description 定時器觸發，更新提示文字
     */
    void updateFrame();
    
    /**
     * @brief 註冊按鈕點擊事件處理
     * @description 將輸入姓名寫入資料庫
     */
    void on_pushButton_register_clicked();
    
    /**
     * @brief 刪除按鈕點擊事件處理
     * @description 從資料庫中刪除指定使用者
     */
    void on_pushButton_DELETE_clicked();

private:
    Ui::MainWindow *ui;          // UI 介面指標

    // 資料庫相關
    QSqlDatabase db;             // SQLite 資料庫連線
    QTimer *timer = nullptr;     // 狀態更新定時器
};
