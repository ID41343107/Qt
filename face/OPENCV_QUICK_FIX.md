# OpenCV Configuration Quick Reference

## For Windows Users Experiencing Build Errors

### Problem
```
error: 'opencv2/opencv.hpp' file not found
```

### Solution (3 Steps)

#### Step 1: Set Environment Variables
1. Press `Win + X` → System → Advanced system settings → Environment Variables
2. Add these variables (User or System):
   - **Variable:** `OPENCV_DIR`  
     **Value:** `C:\opencv\build` (your path)
   - **Variable:** `OPENCV_VERSION`  
     **Value:** `480` (find your version - see below)

#### Step 2: Find Your Version Number
Open folder: `C:\opencv\build\x64\mingw\lib` (or `vc16\lib` for MSVC)
Look for files like:
- `libopencv_world470.a` → version = **470**
- `libopencv_world480.a` → version = **480**
- `libopencv_world4120.a` → version = **4120**

#### Step 3: Rebuild
1. **Restart Qt Creator** (important!)
2. Build → Run qmake
3. Build → Rebuild All

### Still Not Working?

Check these paths exist:
- ✅ `C:\opencv\build\include\opencv2\opencv.hpp`
- ✅ `C:\opencv\build\x64\mingw\lib\` (for MinGW) OR
- ✅ `C:\opencv\build\x64\vc16\lib\` (for MSVC)

If paths are different, update `OPENCV_DIR` to match your installation.

### More Help
See **WINDOWS_SETUP.md** for detailed troubleshooting.

---

## Alternative: Edit face.pro Directly

If you prefer not to use environment variables:

1. Open `face/face.pro`
2. Around line 52-53, uncomment and edit:
```qmake
OPENCV_DIR = C:/your/path/to/opencv/build
OPENCV_VERSION = 480
```
3. Save and rebuild

---

## Quick Verification Checklist

Before building, verify:
- [ ] OpenCV is installed
- [ ] OPENCV_DIR environment variable is set (or edited in face.pro)
- [ ] OPENCV_VERSION environment variable is set (or edited in face.pro)
- [ ] OpenCV version matches your Qt compiler (MinGW vs MSVC)
- [ ] Qt Creator has been restarted
- [ ] Ran "Run qmake" before building

---

## Need to Download OpenCV?

Download from: https://opencv.org/releases/
- Choose a version with pre-built binaries (e.g., 4.8.0, 4.10.0)
- Extract to `C:\opencv\`
- Make sure it has the folder structure:
  ```
  C:\opencv\
    └─ build\
        ├─ include\
        └─ x64\
  ```
