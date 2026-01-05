/**
 * @file main.cpp
 * @brief 人臉辨識門禁系統主程式進入點
 * 
 * 此檔案包含應用程式的 main 函數，負責初始化 Qt 應用程式並顯示主視窗。
 */

#include "mainwindow.h"

#include <QApplication>

/**
 * @brief 程式主要進入點
 * @param argc 命令列參數數量
 * @param argv 命令列參數陣列
 * @return 程式執行結果碼
 * 
 * 建立 Qt 應用程式實例，初始化主視窗並顯示，然後進入事件迴圈。
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);  // 建立 Qt 應用程式物件
    MainWindow w;                 // 建立主視窗物件
    w.show();                     // 顯示主視窗
    return a.exec();              // 進入 Qt 事件迴圈，直到程式結束
}
