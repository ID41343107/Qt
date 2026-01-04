# Windows Setup Guide for OpenCV Configuration

This guide will help you fix the `'opencv2/opencv.hpp' file not found` error on Windows.

## Quick Fix

The error occurs because the project cannot find your OpenCV installation. Follow these steps:

### Step 1: Install OpenCV

1. Download OpenCV for Windows from https://opencv.org/releases/
2. Download a version that includes pre-built libraries (e.g., OpenCV 4.8.0 or 4.12.0)
3. Extract the archive to `C:\opencv\` (or any location you prefer)

After extraction, you should have a structure like:
```
C:\opencv\
  └─ build\
      ├─ include\
      │   └─ opencv2\
      ├─ x64\
      │   ├─ vc16\    (for MSVC)
      │   └─ mingw\   (for MinGW)
```

### Step 2: Configure Environment Variables (RECOMMENDED)

Setting environment variables is the easiest way to configure OpenCV:

1. **Open Environment Variables Settings:**
   - Press `Win + X` and select "System"
   - Click "Advanced system settings"
   - Click "Environment Variables"

2. **Add OPENCV_DIR:**
   - Under "User variables" or "System variables", click "New"
   - Variable name: `OPENCV_DIR`
   - Variable value: `C:\opencv\build` (or your actual path)
   - Click "OK"

3. **Add OPENCV_VERSION:**
   - Click "New" again
   - Variable name: `OPENCV_VERSION`
   - Variable value: Find your version from step 3 below
   - Click "OK"

4. **Add OpenCV to PATH (Optional but recommended):**
   - Find the "Path" variable and click "Edit"
   - Click "New"
   - Add: `C:\opencv\build\x64\mingw\bin` (for MinGW) or `C:\opencv\build\x64\vc16\bin` (for MSVC)
   - Click "OK"

5. **Restart Qt Creator** (important!) to pick up the new environment variables

### Step 3: Find Your OpenCV Version Number

The version number is embedded in the library file names. Look in your OpenCV library folder:

**For MinGW:** Check `C:\opencv\build\x64\mingw\lib\`
- If you see `libopencv_world470.a` → your version is **470**
- If you see `libopencv_world480.a` → your version is **480**
- If you see `libopencv_world4120.a` → your version is **4120**

**For MSVC:** Check `C:\opencv\build\x64\vc16\lib\`
- If you see `opencv_world470d.lib` or `opencv_world470.lib` → your version is **470**
- If you see `opencv_world480d.lib` or `opencv_world480.lib` → your version is **480**
- If you see `opencv_world4120d.lib` or `opencv_world4120.lib` → your version is **4120**
- Note: Files ending with 'd' are debug libraries, without 'd' are release libraries

### Step 4: Verify Compiler Match

Make sure your OpenCV build matches your Qt compiler:

**In Qt Creator:**
1. Go to "Tools" → "Options" → "Kits"
2. Check which compiler your kit uses:
   - **MinGW 64-bit** → You need OpenCV built with MinGW (in `x64/mingw/` folder)
   - **MSVC 2019/2022** → You need OpenCV built with MSVC (in `x64/vc16/` folder)

**If your OpenCV doesn't have the matching compiler folder:**
- Either switch your Qt kit to use a different compiler
- Or download a different OpenCV package that includes your compiler
- Or build OpenCV from source (advanced)

## Alternative Method: Edit face.pro Directly

If you don't want to use environment variables, you can edit the `face.pro` file:

1. Open `face/face.pro` in a text editor
2. Find the Windows OpenCV configuration section (around line 43)
3. Uncomment and modify these lines:
   ```qmake
   OPENCV_DIR = C:/your/opencv/path/build
   OPENCV_VERSION = 480  # Your version number
   ```
4. Save the file

## Troubleshooting

### Error: "OpenCV include directory not found"
- **Cause:** OPENCV_DIR is not set correctly
- **Solution:** Double-check the path. It should point to the `build` folder, not the root opencv folder
- **Example:** `C:/opencv/build` (correct) vs `C:/opencv` (wrong)

### Error: "OpenCV library directory not found"
- **Cause:** Your OpenCV installation doesn't have libraries for your compiler
- **Solution:** 
  - Check if `C:/opencv/build/x64/mingw/lib` exists (for MinGW)
  - Check if `C:/opencv/build/x64/vc16/lib` exists (for MSVC)
  - If missing, download a different OpenCV package or build from source

### Error: "Unknown Windows compiler. Trying MinGW paths."
- **Cause:** Qt cannot detect your compiler type automatically
- **Impact:** The build will try MinGW paths by default, which may work if you're using MinGW
- **Solution:** If you're using MSVC, make sure you open the project with the MSVC kit selected in Qt Creator

### Build succeeds but exe crashes immediately
- **Cause:** OpenCV DLL files are not in PATH
- **Solution:** Add OpenCV bin folder to PATH:
  - For MinGW: `C:\opencv\build\x64\mingw\bin`
  - For MSVC: `C:\opencv\build\x64\vc16\bin`

### Still having issues?
1. Open Qt Creator
2. Go to "Build" → "Run qmake"
3. Go to "Build" → "Rebuild All"
4. Check the "Compile Output" pane for detailed messages about OpenCV configuration
5. Look for these lines:
   ```
   OpenCV Configuration:
     OPENCV_DIR = C:/opencv/build
     OPENCV_VERSION = 480
     Compiler: MinGW (g++)
     Library Dir: C:/opencv/build/x64/mingw/lib
   ```
6. Verify each path exists on your system

## Example: Complete Setup

Here's a complete example assuming you downloaded OpenCV 4.8.0:

1. Extract to `C:\opencv\`
2. Set environment variables:
   - `OPENCV_DIR = C:\opencv\build`
   - `OPENCV_VERSION = 480`
3. Add to PATH: `C:\opencv\build\x64\mingw\bin`
4. Restart Qt Creator
5. Open project → Run qmake → Build

## Need More Help?

If you're still stuck:
1. Take a screenshot of the "Compile Output" pane showing the OpenCV configuration messages
2. Check what files exist in your OpenCV installation folders
3. Open an issue on GitHub with:
   - Your OpenCV version and installation path
   - Your Qt Creator compiler kit (MinGW or MSVC)
   - The complete error message
   - The OpenCV configuration output from the build log
