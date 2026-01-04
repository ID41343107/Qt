/**
 * @file main.cpp
 * @brief 人臉辨識系統主程式入口
 * @description 基於 Qt 和 OpenCV 的人臉辨識門禁系統
 */

#include "mainwindow.h"

#include <QApplication>

/**
 * @brief 應用程式主函數
 * @param argc 命令列參數數量
 * @param argv 命令列參數陣列
 * @return 應用程式執行結果
 */
int main(int argc, char *argv[])
{
    // 建立 Qt 應用程式實例
    QApplication a(argc, argv);
    
    // 建立並顯示主視窗
    MainWindow w;
    w.show();
    
    // 進入事件循環
    return a.exec();
}
