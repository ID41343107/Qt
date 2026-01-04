# Face Recognition Access Control System

基於 Qt 和 OpenCV 的人臉辨識門禁系統

## 系統需求

- Qt 6.x 或更高版本
- OpenCV 4.x 或更高版本（需含 DNN 模組）
- SQLite3（Qt 已內建）
- C++17 編譯器
- 網路攝影機

## 安裝說明

### 1. 安裝 Qt

#### Windows
從 [Qt 官網](https://www.qt.io/download) 下載並安裝 Qt 6.x

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-tools-dev-tools
sudo apt-get install libqt6sql6-sqlite
```

### 2. 安裝 OpenCV

#### Windows
1. 從 [OpenCV 官網](https://opencv.org/releases/) 下載 OpenCV 4.x
2. 解壓縮到 `C:/opencv/`
3. 將 OpenCV bin 目錄加入系統 PATH

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libopencv-dev
```

或從源碼編譯最新版本：
```bash
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j$(nproc)
sudo make install
```

### 3. 下載深度學習模型檔案

此應用程式需要以下模型檔案才能進行人臉偵測與辨識：

1. **res10_300x300_ssd_iter_140000.caffemodel**
   - 人臉偵測模型（Caffe SSD）
   - 下載連結：[OpenCV Face Detector](https://github.com/opencv/opencv_3rdparty/raw/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel)

2. **deploy.prototxt**
   - 人臉偵測網路架構定義
   - 下載連結：[Deploy Prototxt](https://github.com/opencv/opencv/raw/master/samples/dnn/face_detector/deploy.prototxt)

3. **openface_nn4.small2.v1.t7**
   - 人臉特徵提取模型（OpenFace）
   - 下載連結：[OpenFace Model](https://github.com/pyannote/pyannote-data/raw/master/openface.nn4.small2.v1.t7)

**下載指令範例（Linux/macOS）：**
```bash
cd face/
wget https://github.com/opencv/opencv_3rdparty/raw/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel
wget https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt
wget https://github.com/pyannote/pyannote-data/raw/master/openface.nn4.small2.v1.t7
```

**注意：** 將所有模型檔案放置在編譯後執行檔所在目錄的 `face/` 子資料夾中。

## 編譯與執行

### 方法一：使用 qmake 編譯

```bash
cd face/
qmake face.pro
make
```

### 方法二：使用 CMake 編譯（推薦）

```bash
cd face/
mkdir build
cd build
cmake ..
make
```

### 方法三：使用 Qt Creator

1. 開啟 Qt Creator
2. 選擇「開啟專案」
3. 選擇 `face/face.pro` 或 `face/CMakeLists.txt` 檔案
4. 設定建置套件（Kit）
5. 點擊「建置」按鈕
6. 點擊「執行」按鈕

### Windows 使用 MSVC

如果使用 MSVC 編譯器，可以執行提供的批次檔：
```cmd
cd face\
msvc_make.bat
```

或使用 CMake：
```cmd
cd face\
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 功能說明

### 1. 即時攝影機預覽
- 開啟應用程式後會自動啟動攝影機
- 主畫面會顯示即時影像串流

### 2. 人臉註冊
1. 在「註冊姓名」欄位輸入使用者姓名
2. 確保臉部清晰出現在攝影機畫面中
3. 點擊「註冊使用者」按鈕
4. 系統會自動偵測人臉並儲存特徵到資料庫

### 3. 人臉辨識與視覺回饋
- 當偵測到人臉時，會在畫面上顯示**紅色方框**
- 若辨識成功（資料庫中的已註冊使用者）：
  - 前 3 秒：方框保持**紅色**
  - 3 秒後：方框變為**綠色**
  - 同時將「友人到」訊息自動寫入 `work/友人到.txt` 檔案
  - 檔案包含時間戳記和使用者 ID
- 若未辨識或陌生人：方框保持**紅色**
- 門禁會在辨識成功後開啟 3 秒後自動關閉

### 4. 刪除使用者
1. 在「刪除姓名」欄位輸入要刪除的使用者姓名
2. 點擊「刪除使用者」按鈕
3. 系統會從資料庫中移除該使用者資料

## 注意事項

### 攝影機無法開啟
- 確認攝影機已正確連接並啟用
- 檢查其他應用程式是否正在使用攝影機
- Linux 系統需確認使用者有攝影機存取權限：
  ```bash
  sudo usermod -a -G video $USER
  ```

### 模型檔案未載入
- 確認模型檔案已下載並放置在正確位置
- 檢查檔案路徑：執行檔目錄下的 `face/` 資料夾
- 查看終端機輸出的錯誤訊息以確認問題

### OpenCV 編譯錯誤
- 確認已安裝正確版本的 OpenCV
- Linux 系統可能需要安裝 pkg-config：
  ```bash
  sudo apt-get install pkg-config
  ```
- 檢查 `.pro` 檔案中的 OpenCV 路徑設定

### Windows 連結器錯誤 (ld returned 1 exit status)

這是最常見的 Windows 編譯問題。請依序檢查以下項目：

#### 1. 確認 OpenCV 已正確安裝
檢查以下路徑是否存在：
- MinGW 版本：`C:/opencv/build/x64/mingw/lib/libopencv_world4120d.a`
- MSVC 版本：`C:/opencv/build/x64/vc16/lib/opencv_world4120d.lib`

如果路徑不存在或版本號不同（如 470、480），請：

**方法一：調整 face.pro 檔案**
1. 開啟 `face/face.pro`
2. 找到這幾行（約第 30-35 行）：
   ```qmake
   isEmpty(OPENCV_DIR) {
       OPENCV_DIR = C:/opencv/build
   }
   isEmpty(OPENCV_VERSION) {
       OPENCV_VERSION = 4120
   }
   ```
3. 修改 `OPENCV_DIR` 為您的 OpenCV 安裝路徑
4. 修改 `OPENCV_VERSION` 為您的版本號（如 `470` 或 `480`）

**方法二：下載正確的 OpenCV 版本**
- 從 [OpenCV Releases](https://opencv.org/releases/) 下載 Windows 版本
- 確保下載的版本包含您使用的編譯器（MinGW 或 MSVC）
- 解壓縮到 `C:/opencv`

#### 2. 確認編譯器類型匹配
在 Qt Creator 中：
1. 查看「專案」→「建置套件」
2. 確認使用的編譯器（MinGW 或 MSVC）
3. 確保 OpenCV 版本與編譯器匹配：
   - **MinGW 64-bit** → 使用 `opencv/build/x64/mingw/lib`
   - **MSVC 2019/2022** → 使用 `opencv/build/x64/vc16/lib`

#### 3. 執行完整重建
1. 在 Qt Creator 中：「建置」→「清除全部」
2. 「建置」→「執行 qmake」（重要！）
3. 「建置」→「重建專案」

#### 4. 檢查建置輸出
在「編譯輸出」視窗中查看：
- `Using OpenCV directory: ...` - 確認路徑正確
- `Detected MinGW compiler` 或 `Detected MSVC compiler` - 確認偵測正確
- `OpenCV library directory: ...` - 確認函式庫路徑正確

如果以上都無法解決，請提供：
- 完整的建置輸出訊息
- 您的 OpenCV 安裝路徑
- Qt Creator 使用的編譯器類型

### 辨識準確度問題
- 確保光線充足且均勻
- 臉部需完整且清晰出現在畫面中
- 建議正面拍攝，避免側面或傾斜角度
- 可以嘗試多次註冊同一人以提高辨識率

## 資料庫說明

- 使用 SQLite 資料庫儲存使用者資料
- 資料庫檔案：`users.db`（自動在執行目錄建立）
- 每位使用者儲存 128 維的人臉特徵向量
- 資料表結構：
  - `id`: 使用者 ID（自動遞增）
  - `name`: 使用者姓名
  - `v1` ~ `v128`: 人臉特徵向量的 128 個維度

## 輸出檔案說明

### work 資料夾
- 自動建立於執行檔所在目錄
- 儲存人臉辨識的記錄檔案

### 友人到.txt
- 路徑：`work/友人到.txt`
- 當辨識到已註冊使用者且持續 3 秒後自動寫入
- 檔案格式：
  ```
  友人到 - 2026-01-04 12:34:56 (ID: 1)
  友人到 - 2026-01-04 13:45:23 (ID: 2)
  ```
- 每次辨識僅記錄一次，直到該使用者離開並重新被辨識

## 技術細節

### 人臉偵測
- 使用 Caffe SSD 模型進行人臉偵測
- 輸入影像尺寸：300x300
- 偵測信心值閾值：0.6

### 人臉辨識
- 使用 OpenFace 模型提取 128 維特徵向量
- 輸入人臉尺寸：96x96 RGB
- 辨識採用歐式距離比對
- 距離閾值：0.8（距離越小表示越相似）

## 授權

請參閱專案中的 LICENSE 檔案。

## 相關資源

- [Qt 官方文件](https://doc.qt.io/)
- [OpenCV 官方文件](https://docs.opencv.org/)
- [Caffe SSD Face Detector](https://github.com/opencv/opencv/tree/master/samples/dnn/face_detector)
- [OpenFace](https://cmusatyalab.github.io/openface/)

## 疑難排解

如遇到問題，請檢查：
1. Qt 和 OpenCV 是否正確安裝
2. 模型檔案是否完整下載
3. 攝影機權限是否正確設定
4. 查看應用程式終端機輸出的除錯訊息

如需協助，請在專案 GitHub 頁面提出 Issue。
