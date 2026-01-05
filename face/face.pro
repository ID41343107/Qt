QT += core gui sql network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android:  target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# ==================== OpenCV MinGW 配置 ====================
INCLUDEPATH += C:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/include

LIBS += -LC:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib

# 使用完整的库文件路径
LIBS += C:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib/libopencv_core455.dll.a \
        C:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib/libopencv_imgproc455.dll.a \
        C:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib/libopencv_highgui455.dll.a \
        C:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib/libopencv_imgcodecs455.dll.a \
        C:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib/libopencv_videoio455.dll.a \
        C:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib/libopencv_dnn455.dll.a

LIBS += -LC:/opencv-mingw/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/bin
