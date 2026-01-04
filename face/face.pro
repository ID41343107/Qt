QT       += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
INCLUDEPATH += C:/opencv/build/include

# Cross-platform OpenCV configuration
unix {
    # Linux/Unix: Try to use pkg-config first
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4
    
    # Fallback to manual configuration if pkg-config fails
    !packagesExist(opencv4) {
        INCLUDEPATH += /usr/local/include/opencv4
        LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui \
                -lopencv_imgcodecs -lopencv_videoio -lopencv_dnn
    }
}

# MinGW configuration
win32-g++ {
    INCLUDEPATH += C:/opencv/build/include
    CONFIG(debug, debug|release) {
        LIBS += -LC:/opencv/build/x64/mingw/lib \
                -lopencv_world4120d
    } else {
        LIBS += -LC:/opencv/build/x64/mingw/lib \
                -lopencv_world4120
    }
}

# MSVC configuration
win32-msvc* {
    INCLUDEPATH += C:/opencv/build/include
    CONFIG(debug, debug|release) {
        LIBS += -LC:/opencv/build/x64/vc16/lib \
                -lopencv_world4120d
    } else {
        LIBS += -LC:/opencv/build/x64/vc16/lib \
                -lopencv_world4120
    }
}
