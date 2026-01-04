@echo off
REM 下載人臉辨識所需的深度學習模型檔案

echo 正在下載人臉辨識模型檔案...
echo =========================================

cd /d "%~dp0"

REM 檢查是否安裝 curl 或 PowerShell
where curl >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    set DOWNLOAD_CMD=curl
) else (
    set DOWNLOAD_CMD=powershell
)

REM 1. 下載人臉偵測模型 (Caffe SSD)
echo 1/3 正在下載 res10_300x300_ssd_iter_140000.caffemodel...
if exist "res10_300x300_ssd_iter_140000.caffemodel" (
    echo    檔案已存在，跳過下載
) else (
    if "%DOWNLOAD_CMD%"=="curl" (
        curl -L -o res10_300x300_ssd_iter_140000.caffemodel https://github.com/opencv/opencv_3rdparty/raw/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel
    ) else (
        powershell -Command "Invoke-WebRequest -Uri 'https://github.com/opencv/opencv_3rdparty/raw/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel' -OutFile 'res10_300x300_ssd_iter_140000.caffemodel'"
    )
    if exist "res10_300x300_ssd_iter_140000.caffemodel" (
        echo    √ 下載成功
    ) else (
        echo    × 下載失敗
    )
)

REM 2. 下載網路架構定義檔案
echo 2/3 正在下載 deploy.prototxt...
if exist "deploy.prototxt" (
    echo    檔案已存在，跳過下載
) else (
    if "%DOWNLOAD_CMD%"=="curl" (
        curl -L -o deploy.prototxt https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt
    ) else (
        powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt' -OutFile 'deploy.prototxt'"
    )
    if exist "deploy.prototxt" (
        echo    √ 下載成功
    ) else (
        echo    × 下載失敗
    )
)

REM 3. 下載人臉特徵提取模型 (OpenFace)
echo 3/3 正在下載 openface_nn4.small2.v1.t7...
if exist "openface_nn4.small2.v1.t7" (
    echo    檔案已存在，跳過下載
) else (
    if "%DOWNLOAD_CMD%"=="curl" (
        curl -L -o openface_nn4.small2.v1.t7 https://github.com/pyannote/pyannote-data/raw/master/openface.nn4.small2.v1.t7
    ) else (
        powershell -Command "Invoke-WebRequest -Uri 'https://github.com/pyannote/pyannote-data/raw/master/openface.nn4.small2.v1.t7' -OutFile 'openface_nn4.small2.v1.t7'"
    )
    if exist "openface_nn4.small2.v1.t7" (
        echo    √ 下載成功
    ) else (
        echo    × 下載失敗
    )
)

echo =========================================
echo 模型檔案下載完成！
echo.
echo 已下載的檔案：
dir /b res10_300x300_ssd_iter_140000.caffemodel deploy.prototxt openface_nn4.small2.v1.t7 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo 某些檔案未成功下載，請檢查網路連線並重試
)

pause
