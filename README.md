# Qt Face Recognition System

基於 Qt 和 OpenCV 的人臉辨識門禁系統 (Face Recognition Access Control System)

## 快速開始 (Quick Start)

### 1. 克隆專案
```bash
git clone https://github.com/ID41343107/Qt.git
cd Qt/face
```

### 2. 下載模型檔案
```bash
# Linux/macOS
./download_models.sh

# Windows
download_models.bat
```

### 3. 編譯並執行
```bash
# 使用 CMake (推薦)
mkdir build && cd build
cmake ..
make
./face

# 或使用 qmake
qmake face.pro
make
./face
```

## 詳細說明

完整的安裝與使用說明請參閱 [face/README.md](face/README.md)

## 功能特色

- ✅ 即時人臉偵測與辨識
- ✅ 使用者註冊與管理
- ✅ SQLite 資料庫儲存
- ✅ 深度學習模型 (Caffe SSD + OpenFace)
- ✅ 跨平台支援 (Windows/Linux/macOS)
- ✅ 門禁控制模擬

## 系統需求

- Qt 6.x 或更高版本
- OpenCV 4.x (含 DNN 模組)
- C++17 編譯器
- 網路攝影機

## 授權

請參閱 LICENSE.txt

## 相關連結

- [詳細文件](face/README.md)
- [OpenCV 官網](https://opencv.org/)
- [Qt 官網](https://www.qt.io/)
