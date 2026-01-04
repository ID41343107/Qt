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

# ============================================================================
# OpenCV Library Configuration
# ============================================================================
# This section configures OpenCV linking for different platforms.
# Make sure OpenCV is properly installed before building the project.
# ============================================================================

# ----------------------------------------------------------------------------
# Linux Configuration
# ----------------------------------------------------------------------------
# Uses pkg-config to automatically detect OpenCV installation
# Requires: libopencv-dev package installed via apt-get
unix:!macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4
}

# ----------------------------------------------------------------------------
# Windows Configuration
# ----------------------------------------------------------------------------
# Important: Verify the following paths match your OpenCV installation!
# Default installation path: C:/opencv/
# If you installed OpenCV elsewhere, update all paths below accordingly.
#
# Required directory structure:
#   C:/opencv/build/include/           - Header files
#   C:/opencv/build/x64/vc16/lib/      - Library files (.lib)
#   C:/opencv/build/x64/vc16/bin/      - DLL files (must be in PATH)
#
# Compiler compatibility:
#   - vc16 = Visual Studio 2019
#   - vc15 = Visual Studio 2017  (change path if using this)
#   - mingw = MinGW compiler     (change to x64/mingw if using this)
# ----------------------------------------------------------------------------

win32 {
    # Include path for OpenCV headers
    INCLUDEPATH += C:/opencv/build/include
}

# Debug build configuration for Windows
# Library name format: opencv_worldXXXXd.lib where XXXX is version number
# Example: opencv_world4120d = OpenCV 4.12.0 Debug version
win32:CONFIG(debug, debug|release) {
    LIBS += -LC:/opencv/build/x64/vc16/lib \
            -lopencv_world4120d
}

# Release build configuration for Windows  
# Library name format: opencv_worldXXXX.lib where XXXX is version number
# Example: opencv_world4120 = OpenCV 4.12.0 Release version
win32:CONFIG(release, debug|release) {
    LIBS += -LC:/opencv/build/x64/vc16/lib \
            -lopencv_world4120
}

# ----------------------------------------------------------------------------
# macOS Configuration
# ----------------------------------------------------------------------------
# Uses pkg-config with Homebrew OpenCV installation
# Install OpenCV via: brew install opencv
# ----------------------------------------------------------------------------
macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4
}
