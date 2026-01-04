/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *label_camera;
    QLabel *label_status;
    QLineEdit *lineEdit_name;
    QPushButton *pushButton_register;
    QPushButton *pushButton_DELETE;
    QLineEdit *lineEdit_DELETNAME;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(803, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 10, 221, 19));
        label_camera = new QLabel(centralwidget);
        label_camera->setObjectName("label_camera");
        label_camera->setGeometry(QRect(30, 60, 761, 401));
        label_status = new QLabel(centralwidget);
        label_status->setObjectName("label_status");
        label_status->setGeometry(QRect(290, 0, 261, 51));
        lineEdit_name = new QLineEdit(centralwidget);
        lineEdit_name->setObjectName("lineEdit_name");
        lineEdit_name->setGeometry(QRect(440, 10, 113, 27));
        pushButton_register = new QPushButton(centralwidget);
        pushButton_register->setObjectName("pushButton_register");
        pushButton_register->setGeometry(QRect(560, 10, 96, 28));
        pushButton_DELETE = new QPushButton(centralwidget);
        pushButton_DELETE->setObjectName("pushButton_DELETE");
        pushButton_DELETE->setGeometry(QRect(40, 460, 96, 28));
        lineEdit_DELETNAME = new QLineEdit(centralwidget);
        lineEdit_DELETNAME->setObjectName("lineEdit_DELETNAME");
        lineEdit_DELETNAME->setGeometry(QRect(150, 460, 113, 27));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 803, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "camera preview", nullptr));
        label_camera->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_status->setText(QCoreApplication::translate("MainWindow", "open door", nullptr));
        pushButton_register->setText(QCoreApplication::translate("MainWindow", "\350\250\273\345\206\212\344\275\277\347\224\250\350\200\205", nullptr));
        pushButton_DELETE->setText(QCoreApplication::translate("MainWindow", "\345\210\252\351\231\244\344\275\277\347\224\250\350\200\205", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
