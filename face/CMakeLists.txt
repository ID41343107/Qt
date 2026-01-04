cmake_minimum_required(VERSION 3.16)
project(face VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 尋找 Qt6 套件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# 原始碼檔案
set(SOURCES
    main.cpp
    mainwindow.cpp
)

# 標頭檔
set(HEADERS
    mainwindow.h
)

# UI 檔案
set(FORMS
    mainwindow.ui
)

# 建立執行檔
add_executable(face
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
)

# 連結函式庫
target_link_libraries(face PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

# 設定 include 目錄
target_include_directories(face PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 安裝規則
install(TARGETS face
    RUNTIME DESTINATION bin
)

# 複製模型檔案到建置目錄
