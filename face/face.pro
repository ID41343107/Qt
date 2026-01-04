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

# Windows OpenCV configuration
# ============================================================================
# IMPORTANT: You MUST configure OpenCV paths before building!
# ============================================================================
# 
# Method 1: Set environment variables (RECOMMENDED)
#   Set these in Windows System Environment Variables:
#   - OPENCV_DIR = C:/opencv/build  (or your OpenCV installation path)
#   - OPENCV_VERSION = 4120         (your OpenCV version, e.g., 470, 480, 4120)
#
# Method 2: Edit this file directly
#   Uncomment and modify the lines below:
#   OPENCV_DIR = C:/opencv/build
#   OPENCV_VERSION = 4120
#
# How to find your OpenCV version:
#   Look in your OpenCV lib folder for files like:
#   - libopencv_world470.a  -> version is 470
#   - opencv_world480d.lib  -> version is 480
#   - libopencv_world4120.a -> version is 4120
# ============================================================================
win32 {
    # Check for environment variables first, then use defaults
    isEmpty(OPENCV_DIR) {
        OPENCV_DIR = $$(OPENCV_DIR)
    }
    isEmpty(OPENCV_DIR) {
        OPENCV_DIR = C:/opencv/build
        warning("OPENCV_DIR not set. Using default: $$OPENCV_DIR")
        warning("If build fails, please set OPENCV_DIR environment variable or edit face.pro")
    }
    
    isEmpty(OPENCV_VERSION) {
        OPENCV_VERSION = $$(OPENCV_VERSION)
    }
    isEmpty(OPENCV_VERSION) {
        OPENCV_VERSION = 4120
        warning("OPENCV_VERSION not set. Using default: $$OPENCV_VERSION")
        warning("If build fails, please set OPENCV_VERSION environment variable or edit face.pro")
    }
    
    message("=========================================")
    message("OpenCV Configuration:")
    message("  OPENCV_DIR = $$OPENCV_DIR")
    message("  OPENCV_VERSION = $$OPENCV_VERSION")
    
    # Add OpenCV include directory
    INCLUDEPATH += $$OPENCV_DIR/include
    
    # Detect compiler type and set library paths
    win32-msvc* {
        message("  Compiler: MSVC")
        OPENCV_LIB_DIR = $$OPENCV_DIR/x64/vc16/lib
        CONFIG(debug, debug|release) {
            LIBS += -L$$OPENCV_LIB_DIR -lopencv_world$${OPENCV_VERSION}d
        } else {
            LIBS += -L$$OPENCV_LIB_DIR -lopencv_world$${OPENCV_VERSION}
        }
    } else:win32-g++ {
        message("  Compiler: MinGW (g++)")
        OPENCV_LIB_DIR = $$OPENCV_DIR/x64/mingw/lib
        CONFIG(debug, debug|release) {
            LIBS += -L$$OPENCV_LIB_DIR -lopencv_world$${OPENCV_VERSION}d
        } else {
            LIBS += -L$$OPENCV_LIB_DIR -lopencv_world$${OPENCV_VERSION}
        }
    } else {
        message("  Compiler: Unknown (trying MinGW paths)")
        OPENCV_LIB_DIR = $$OPENCV_DIR/x64/mingw/lib
        CONFIG(debug, debug|release) {
            LIBS += -L$$OPENCV_LIB_DIR -lopencv_world$${OPENCV_VERSION}d
        } else {
            LIBS += -L$$OPENCV_LIB_DIR -lopencv_world$${OPENCV_VERSION}
        }
    }
    
    message("  Library Dir: $$OPENCV_LIB_DIR")
    message("=========================================")
    
    # Verify paths exist
    !exists($$OPENCV_DIR/include) {
        error("OpenCV include directory not found: $$OPENCV_DIR/include. Please install OpenCV or set OPENCV_DIR correctly!")
    }
    !exists($$OPENCV_LIB_DIR) {
        error("OpenCV library directory not found: $$OPENCV_LIB_DIR. Please check OPENCV_DIR and ensure OpenCV is built for your compiler!")
    }
}
