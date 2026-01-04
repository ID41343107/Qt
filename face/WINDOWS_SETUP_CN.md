# Windows OpenCV 配置指南（中文）

本指南將幫助您解決 Windows 上的 `'opencv2/opencv.hpp' file not found` 錯誤。

## 問題說明

您遇到的錯誤訊息：
```
error: OpenCV include directory not found: C:/opencv/build/include
```

這表示專案無法找到 OpenCV 的安裝位置。

## 快速解決方案

### 步驟 1: 下載並安裝 OpenCV

1. **下載 OpenCV**
   - 前往 https://opencv.org/releases/
   - 下載 Windows 版本（建議 4.8.0 或 4.10.0）
   - 選擇包含預編譯函式庫的版本

2. **解壓縮 OpenCV**
   - 將下載的檔案解壓縮到 `C:\opencv\`
   - 解壓後的結構應該如下：
   ```
   C:\opencv\
     └─ build\
         ├─ include\
         │   └─ opencv2\
         ├─ x64\
         │   ├─ vc16\    (MSVC 編譯器用)
         │   └─ mingw\   (MinGW 編譯器用)
   ```

### 步驟 2: 設定環境變數（推薦方法）

1. **開啟環境變數設定**
   - 按 `Win + X` 鍵，選擇「系統」
   - 點擊「進階系統設定」
   - 點擊「環境變數」按鈕

2. **新增 OPENCV_DIR 變數**
   - 在「使用者變數」或「系統變數」中，點擊「新增」
   - 變數名稱：`OPENCV_DIR`
   - 變數值：`C:\opencv\build`（或您實際的安裝路徑）
   - 點擊「確定」

3. **新增 OPENCV_VERSION 變數**
   - 再次點擊「新增」
   - 變數名稱：`OPENCV_VERSION`
   - 變數值：請參考下方「如何找到版本號」
   - 點擊「確定」

4. **新增 OpenCV 到 PATH（選用但建議）**
   - 找到「Path」變數，點擊「編輯」
   - 點擊「新增」
   - 加入：
     - MinGW 用：`C:\opencv\build\x64\mingw\bin`
     - MSVC 用：`C:\opencv\build\x64\vc16\bin`
   - 點擊「確定」

5. **重新啟動 Qt Creator**（重要！）
   - 必須重啟才能讀取新的環境變數

### 步驟 3: 找到您的 OpenCV 版本號

版本號寫在函式庫檔案名稱中：

**MinGW 編譯器：** 查看 `C:\opencv\build\x64\mingw\lib\`
- 如果看到 `libopencv_world470.a` → 版本是 **470**（對應 OpenCV 4.7.0）
- 如果看到 `libopencv_world480.a` → 版本是 **480**（對應 OpenCV 4.8.0）
- 如果看到 `libopencv_world4120.a` → 版本是 **4120**（對應 OpenCV 4.12.0）

**MSVC 編譯器：** 查看 `C:\opencv\build\x64\vc16\lib\`
- 如果看到 `opencv_world470d.lib` → 版本是 **470**
- 如果看到 `opencv_world480d.lib` → 版本是 **480**
- 如果看到 `opencv_world4120d.lib` → 版本是 **4120**
- 註：檔名結尾有 'd' 的是除錯版本，沒有 'd' 的是發行版本

### 步驟 4: 重新編譯

1. 開啟 Qt Creator
2. 點擊「建置」→「執行 qmake」
3. 點擊「建置」→「重建專案」

建置時應該會看到：
```
=========================================
OpenCV Configuration:
  OPENCV_DIR = C:/opencv/build
  OPENCV_VERSION = 480
  Compiler: MinGW (g++)
  Library Dir: C:/opencv/build/x64/mingw/lib
=========================================
```

## 替代方案：直接編輯 face.pro

如果不想使用環境變數，可以直接修改設定檔：

1. 用文字編輯器開啟 `face/face.pro`
2. 找到第 52-53 行左右，取消註解並修改：
   ```qmake
   OPENCV_DIR = C:/您的/opencv/安裝路徑/build
   OPENCV_VERSION = 480
   ```
3. 儲存檔案
4. 在 Qt Creator 中執行 qmake 並重建

## 常見問題

### Q1: 錯誤「OpenCV include directory not found」
**原因：** OPENCV_DIR 設定不正確或 OpenCV 未安裝  
**解決：** 
- 確認 OpenCV 已解壓縮到指定位置
- 確認 `C:\opencv\build\include\opencv2\` 資料夾存在
- 檢查環境變數 OPENCV_DIR 的值是否正確

### Q2: 錯誤「OpenCV library directory not found」
**原因：** OpenCV 安裝版本與您的編譯器不符  
**解決：**
- MinGW 用戶：確認 `C:\opencv\build\x64\mingw\lib` 存在
- MSVC 用戶：確認 `C:\opencv\build\x64\vc16\lib` 存在
- 如果資料夾不存在，請下載包含您編譯器的 OpenCV 版本

### Q3: 編譯成功但執行時閃退
**原因：** OpenCV DLL 檔案不在系統 PATH 中  
**解決：** 將 OpenCV 的 bin 資料夾加入 PATH（參考步驟 2.4）

### Q4: 我的 OpenCV 安裝在其他位置
**解決：** 
- 方法 1：設定環境變數 `OPENCV_DIR` 指向您的安裝路徑的 `build` 資料夾
- 方法 2：直接編輯 `face.pro` 檔案中的 OPENCV_DIR 變數

## 檢查清單

編譯前請確認：
- [ ] OpenCV 已下載並解壓縮
- [ ] 已設定 `OPENCV_DIR` 環境變數（或修改 face.pro）
- [ ] 已設定 `OPENCV_VERSION` 環境變數（或修改 face.pro）
- [ ] OpenCV 版本與 Qt 編譯器相符（MinGW vs MSVC）
- [ ] 已重新啟動 Qt Creator
- [ ] 已執行「執行 qmake」

## 需要更多協助？

如果按照上述步驟仍無法解決問題，請在 GitHub Issues 中提供：
1. 您的 OpenCV 安裝路徑
2. Qt Creator 使用的編譯器類型（MinGW 或 MSVC）
3. 完整的建置錯誤訊息
4. 建置輸出中的 OpenCV 配置訊息

## 參考資源

- [英文詳細指南](WINDOWS_SETUP.md)
- [快速修復指南](OPENCV_QUICK_FIX.md)
- [OpenCV 官方網站](https://opencv.org/)
- [Qt 官方文件](https://doc.qt.io/)
