# Qt Face Detection Application

A Qt-based face detection application using OpenCV for real-time face recognition through webcam.

## Features

- Real-time face detection using webcam
- Face recognition with Haar Cascade classifier
- Automatic door control simulation (opens when face detected)
- Cross-platform support (Linux, Windows, macOS)

## Requirements

### All Platforms
- Qt 6.x or Qt 5.x
- OpenCV 4.x
- C++17 compiler

### Platform-Specific Requirements

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y qt6-base-dev qt6-base-dev-tools build-essential
sudo apt-get install -y libopencv-dev pkg-config
```

#### Windows
1. Install Qt from [Qt Official Website](https://www.qt.io/download)
2. Download OpenCV from [OpenCV Releases](https://opencv.org/releases/)
   - **Note**: The project is currently configured for OpenCV 4.12.0. If using a different version, update the library names in `face.pro` (search for `opencv_world4120` and replace with your version).
3. Extract OpenCV to `C:/opencv/`
4. Ensure the following structure exists:
   - `C:/opencv/build/include/` (header files)
   - `C:/opencv/build/x64/vc16/lib/` (library files)
5. Add OpenCV bin directory to system PATH: `C:/opencv/build/x64/vc16/bin/`

#### macOS
```bash
brew install qt opencv pkg-config
```

## Building the Project

### Linux/macOS
```bash
cd face
qmake6 face.pro  # or qmake-qt5 for Qt5
make
./face
```

### Windows
1. Open Qt Creator
2. Open the `face.pro` project file
3. Configure the project with your Qt kit
4. Build and run

Or using command line:
```cmd
cd face
qmake face.pro
nmake  # or mingw32-make for MinGW
face.exe
```

## Project Structure

```
Qt/
├── face/
│   ├── face.pro                            # Qt project file
│   ├── main.cpp                            # Application entry point
│   ├── mainwindow.cpp/h                    # Main window implementation
│   ├── mainwindow.ui                       # UI design file
│   └── haarcascade_frontalface_default.xml # Face detection model
├── .gitignore                              # Git ignore rules
└── README.md                               # This file
```

## Configuration Details

The `face.pro` file is configured to automatically detect the platform and use appropriate OpenCV settings:

- **Linux**: Uses pkg-config to find OpenCV (opencv4)
- **Windows**: Uses hardcoded paths to `C:/opencv/`
- **macOS**: Uses pkg-config with Homebrew OpenCV

## Troubleshooting

### OpenCV not found on Linux
```bash
pkg-config --modversion opencv4
# Should return version number, e.g., 4.6.0
```

If not found, reinstall:
```bash
sudo apt-get install --reinstall libopencv-dev
```

### Build errors on Windows

#### Linker errors (ld returned 1 exit status)
If you see linker errors like `collect2.exe error: ld returned 1 exit status`, check the following:

1. **Verify OpenCV installation path**:
   - Open `face.pro` and confirm the paths match your OpenCV installation
   - Default path is `C:/opencv/build/`
   - Check that these files exist:
     - `C:/opencv/build/include/opencv2/opencv.hpp`
     - `C:/opencv/build/x64/vc16/lib/opencv_world4120d.lib` (debug)
     - `C:/opencv/build/x64/vc16/lib/opencv_world4120.lib` (release)

2. **Update library version**:
   - If using a different OpenCV version (e.g., 4.8.0), update the library names in `face.pro`
   - Replace `opencv_world4120d` with `opencv_world480d` (debug)
   - Replace `opencv_world4120` with `opencv_world480` (release)

3. **Check compiler compatibility**:
   - The default config uses `x64/vc16` (Visual Studio 2019)
   - For MinGW, change path to `x64/mingw`
   - For different Visual Studio versions: `vc15` (VS 2017), `vc14` (VS 2015)

4. **Verify system PATH**:
   - Add OpenCV DLLs to system PATH: `C:/opencv/build/x64/vc16/bin/`
   - Restart Qt Creator after modifying PATH

#### Other Windows issues
- Ensure Qt can find the compiler (MSVC or MinGW)
- If using Qt from online installer, ensure the correct kit is selected in Qt Creator

### Camera not working
- Linux: Ensure user has access to `/dev/video0`
  ```bash
  sudo usermod -a -G video $USER
  # Then logout and login, or run: newgrp video
  ```
- Windows: Check camera permissions in Windows Settings
- macOS: Grant camera access in System Preferences > Security & Privacy

## License

See LICENSE.txt for details.