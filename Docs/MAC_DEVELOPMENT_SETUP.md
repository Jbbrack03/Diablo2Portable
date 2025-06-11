# Mac Development Setup for Diablo II Android Port

## Mac Studio M3 Ultra Compatibility

The Mac Studio M3 Ultra is fully capable of developing the Android port. Here's what works and what needs attention:

### ✅ Fully Compatible Components

1. **Android Studio**
   - Native Apple Silicon support
   - Excellent performance on M3
   - Full Android SDK/NDK support

2. **Development Tools**
   - CMake (via Homebrew)
   - Git
   - Visual Studio Code / other IDEs
   - Android emulators (ARM64 versions)

3. **Asset Extraction Tools**
   - StormLib compiles natively on macOS
   - Python scripts run without issues
   - Image manipulation tools available

### ⚠️ Compatibility Considerations

1. **Wine for Windows Tools**
   - Original game installers require Wine/CrossOver
   - Some MPQ tools are Windows-only
   - Solution: Use Wine via Homebrew or CrossOver

2. **Android Emulator**
   - ARM64 Android images recommended
   - x86/x86_64 emulation is slower on Apple Silicon
   - Physical device testing preferred

3. **Legacy Windows Tools**
   - Some Diablo II modding tools are Windows-only
   - Most can run via Wine with minor setup

## Installation Guide

### 1. Install Homebrew (if not already installed)
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 2. Install Development Tools
```bash
# Install essential tools
brew install cmake git wget python3

# Install Wine for Windows compatibility
brew install --cask wine-stable

# Optional: CrossOver for better Windows app support
# Download from codeweavers.com
```

### 3. Install Android Studio
```bash
# Download Android Studio for Apple Silicon
# https://developer.android.com/studio

# Or via Homebrew
brew install --cask android-studio
```

### 4. Configure Android SDK/NDK
```bash
# In Android Studio:
# 1. Open SDK Manager
# 2. Install:
#    - Android SDK Platform 33
#    - Android SDK Build-Tools
#    - NDK (Side by side)
#    - CMake
#    - Android Emulator

# Add to ~/.zshrc
export ANDROID_HOME=$HOME/Library/Android/sdk
export PATH=$PATH:$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools
export PATH=$PATH:$ANDROID_HOME/ndk/25.2.9519653
```

### 5. Build StormLib for MPQ Extraction
```bash
# Clone and build StormLib
git clone https://github.com/ladislav-zezula/StormLib.git
cd StormLib
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Install the library
sudo cmake --install build
```

### 6. Set Up Wine for Game Installation
```bash
# Configure Wine prefix
export WINEPREFIX=$HOME/.wine_diablo2
winecfg

# Set Windows version to Windows XP
# Install the game from ISO
```

## Project Structure for Mac Development

```bash
# Create project directory
mkdir -p ~/Development/Diablo2Portable
cd ~/Development/Diablo2Portable

# Initialize Android project
# Use Android Studio's New Project wizard
# Select "Native C++" template
```

## Extracting Game Files on macOS

### Mount ISO Files
```bash
# macOS can mount ISOs natively
hdiutil mount "vendor/Diablo II CD Images/Diablo II install.iso"
hdiutil mount "vendor/Diablo II CD Images/Diablo 2 Lords of Destruction.iso"

# Files will appear in /Volumes/
ls /Volumes/
```

### Run Installers via Wine
```bash
# Install base game
cd "/Volumes/Diablo II"
wine setup.exe

# Install expansion
cd "/Volumes/DIABLO2LOD"
wine installer.exe

# Apply patch
wine "vendor/Diablo II CD Images/LODPatch_114d.exe"
```

### Extract MPQ Files
```bash
# Use StormLib-based extractor
./tools/mpq_extractor \
  --input "$WINEPREFIX/drive_c/Program Files/Diablo II/d2data.mpq" \
  --output ./extracted_assets
```

## Development Workflow on Mac

### 1. Code Development
- Use Android Studio or VS Code
- Full C++ debugging support
- Integrated Android logcat

### 2. Building
```bash
# Build from command line
cd android
./gradlew assembleDebug

# Or use Android Studio's build system
```

### 3. Testing Options

#### A. Physical Device (Recommended)
```bash
# Enable USB debugging on Android device
# Connect via USB
adb devices
adb install app/build/outputs/apk/debug/app-debug.apk
```

#### B. Android Emulator
```bash
# Create ARM64 emulator for best performance
# Use Android Studio's AVD Manager
# Select arm64-v8a system images
```

#### C. Retroid Pocket Flip 2
- Enable ADB over WiFi
- Connect and deploy directly

## Mac-Specific Optimizations

### 1. Parallel Builds
```gradle
// In gradle.properties
org.gradle.workers.max=16  # Adjust based on M3 Ultra cores
```

### 2. Build Caching
```bash
# Enable Gradle build cache
mkdir -p ~/.gradle
echo "org.gradle.caching=true" >> ~/.gradle/gradle.properties
```

### 3. Use Native ARM64 Tools
- Ensure all tools are ARM64 native
- Avoid Rosetta 2 translation when possible

## Troubleshooting

### Issue: Wine Not Running x86 Apps
```bash
# Install Rosetta 2 if needed
softwareupdate --install-rosetta

# Use CrossOver for better compatibility
```

### Issue: Android Emulator Performance
```bash
# Use ARM64 system images only
# Enable hardware acceleration
# Allocate sufficient RAM (8GB+)
```

### Issue: CMake Not Found
```bash
# Ensure CMake from Android SDK is in PATH
export PATH=$PATH:$ANDROID_HOME/cmake/3.22.1/bin
```

## Performance Tips for M3 Ultra

1. **Leverage Multiple Cores**
   - Configure parallel compilation
   - Use all available cores for builds

2. **Fast Storage**
   - Keep project on internal SSD
   - Use build caches effectively

3. **Memory Allocation**
   - Increase Android Studio heap size
   - Allocate more RAM to emulators

## Alternative: Docker Development
```dockerfile
# Optional: Use Docker for Linux environment
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    android-sdk \
    build-essential \
    cmake \
    wine
```

## Summary

The Mac Studio M3 Ultra is perfectly suited for this project:
- ✅ Native Android development support
- ✅ Excellent build performance
- ✅ All required tools available
- ⚠️ Minor Wine setup for Windows tools
- ⚠️ Prefer ARM64 Android images

No significant compatibility issues exist. The main consideration is using Wine for the initial game installation and some Windows-only tools, which works well on macOS.