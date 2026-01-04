#!/bin/bash
# 下載人臉辨識所需的深度學習模型檔案

echo "正在下載人臉辨識模型檔案..."
echo "========================================="

# 建立模型檔案存放目錄
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# 1. 下載人臉偵測模型 (Caffe SSD)
echo "1/3 正在下載 res10_300x300_ssd_iter_140000.caffemodel..."
if [ -f "res10_300x300_ssd_iter_140000.caffemodel" ]; then
    echo "   檔案已存在，跳過下載"
else
    wget -q --show-progress https://github.com/opencv/opencv_3rdparty/raw/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel
    if [ $? -eq 0 ]; then
        echo "   ✓ 下載成功"
    else
        echo "   ✗ 下載失敗"
    fi
fi

# 2. 下載網路架構定義檔案
echo "2/3 正在下載 deploy.prototxt..."
if [ -f "deploy.prototxt" ]; then
    echo "   檔案已存在，跳過下載"
else
    wget -q --show-progress https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt
    if [ $? -eq 0 ]; then
        echo "   ✓ 下載成功"
    else
        echo "   ✗ 下載失敗"
    fi
fi

# 3. 下載人臉特徵提取模型 (OpenFace)
echo "3/3 正在下載 openface_nn4.small2.v1.t7..."
if [ -f "openface_nn4.small2.v1.t7" ]; then
    echo "   檔案已存在，跳過下載"
else
    wget -q --show-progress https://github.com/pyannote/pyannote-data/raw/master/openface.nn4.small2.v1.t7 -O openface_nn4.small2.v1.t7
    if [ $? -eq 0 ]; then
        echo "   ✓ 下載成功"
    else
        echo "   ✗ 下載失敗"
    fi
fi

echo "========================================="
echo "模型檔案下載完成！"
echo ""
echo "已下載的檔案："
ls -lh res10_300x300_ssd_iter_140000.caffemodel deploy.prototxt openface_nn4.small2.v1.t7 2>/dev/null || echo "某些檔案未成功下載，請檢查網路連線並重試"
