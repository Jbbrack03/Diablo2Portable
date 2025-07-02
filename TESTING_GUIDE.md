# Diablo II Android Port - Testing Guide

## 📱 **APK Build and Testing Instructions**

### **🔧 Prerequisites Setup**

#### **1. Android SDK Installation**
```bash
# Install Android Studio (includes Android SDK)
# Or install Android SDK command line tools
brew install android-commandlinetools

# Set environment variables
export ANDROID_HOME=$HOME/Library/Android/sdk
export PATH=$PATH:$ANDROID_HOME/emulator
export PATH=$PATH:$ANDROID_HOME/platform-tools
export PATH=$PATH:$ANDROID_HOME/cmdline-tools/latest/bin

# Accept Android SDK licenses
sdkmanager --licenses
```

#### **2. Java Version Management**
```bash
# Install Java 11 (Android builds prefer Java 8/11 over Java 23)
brew install openjdk@11

# Set JAVA_HOME for Android builds
export JAVA_HOME=/opt/homebrew/opt/openjdk@11
```

#### **3. NDK Setup**
```bash
# Install NDK version specified in build.gradle (27.0.12077973)
sdkmanager "ndk;27.0.12077973"

# Set NDK path
export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/27.0.12077973
```

### **🏗️ Building the APK**

#### **Debug Build (Development Testing)**
```bash
cd android/

# Clean build
./gradlew clean

# Build debug APK
./gradlew assembleDebug

# Output location: app/build/outputs/apk/debug/app-debug.apk
```

#### **Release Build (Performance Testing)**
```bash
# Build release APK (unsigned for testing)
./gradlew assembleRelease

# Output location: app/build/outputs/apk/release/app-release-unsigned.apk
```

### **📱 Device Testing Methods**

#### **Method 1: Android Emulator**
```bash
# Create AVD (Android Virtual Device)
avdmanager create avd -n D2Test -k "system-images;android-33;google_apis;arm64-v8a"

# Start emulator
emulator -avd D2Test

# Install APK
adb install app/build/outputs/apk/debug/app-debug.apk

# Launch app
adb shell am start -n com.diablo2portable/.MainActivity
```

#### **Method 2: Physical Android Device**
```bash
# Enable Developer Options and USB Debugging on device
# Connect device via USB

# Check device connection
adb devices

# Install APK
adb install app/build/outputs/apk/debug/app-debug.apk

# View logs in real-time
adb logcat | grep "D2Portable"
```

#### **Method 3: Retroid Pocket Flip 2 (Target Device)**
```bash
# Enable ADB over Wi-Fi on Retroid device
# Settings > Developer Options > Wireless debugging

# Connect via Wi-Fi
adb connect [DEVICE_IP]:5555

# Install and test
adb install app/build/outputs/apk/debug/app-debug.apk
```

### **🎮 Testing Scenarios**

#### **Controller Testing**
```bash
# Test with Xbox/PlayStation controller connected via Bluetooth
# Or use Retroid device's built-in controls

# Key areas to test:
# - Menu navigation (D-pad/analog stick)
# - Character movement
# - Inventory management
# - Skill tree navigation
# - Combat controls
```

#### **Performance Testing**
```bash
# Monitor performance during gameplay
adb shell top | grep com.diablo2portable

# GPU performance
adb shell dumpsys gfxinfo com.diablo2portable

# Memory usage
adb shell dumpsys meminfo com.diablo2portable
```

#### **Game Asset Testing**
```bash
# Copy D2 MPQ files to device storage
adb push vendor/mpq/ /sdcard/Android/data/com.diablo2portable/files/

# Verify asset loading in app logs
adb logcat | grep "MPQ\|Asset\|Sprite"
```

### **🔍 Debugging and Troubleshooting**

#### **View Application Logs**
```bash
# Real-time logging
adb logcat -s D2Engine:V D2Portable:V

# Save logs to file
adb logcat > d2_test_logs.txt

# Filter for errors
adb logcat | grep -E "(ERROR|FATAL|EXCEPTION)"
```

#### **Performance Profiling**
```bash
# CPU usage
adb shell top -p $(adb shell pidof com.diablo2portable)

# Memory allocation tracking
adb shell dumpsys meminfo com.diablo2portable --package

# GPU frame timing
adb shell dumpsys gfxinfo com.diablo2portable framestats
```

#### **Native Library Debugging**
```bash
# Enable native debugging in AndroidManifest.xml
# <application android:debuggable="true">

# Use ndk-gdb for native debugging
ndk-gdb --project=app/src/main/cpp
```

### **✅ Test Cases to Verify**

#### **Core Functionality**
- [ ] App launches without crashes
- [ ] Main menu displays and navigates properly
- [ ] Game engine initializes successfully
- [ ] MPQ files load (if present)
- [ ] Character creation works
- [ ] Basic gameplay (movement, inventory) functions

#### **Controller Input**
- [ ] Menu navigation with D-pad/analog stick
- [ ] Character movement with analog stick
- [ ] Action button triggers UI interactions
- [ ] Cancel button navigates back properly
- [ ] Inventory grid navigation works

#### **Performance Targets**
- [ ] Maintains 60 FPS during gameplay
- [ ] Memory usage stays under 1.5GB
- [ ] Battery drain is reasonable (4+ hour target)
- [ ] No significant frame drops during combat

#### **UI/UX Validation**
- [ ] Text renders clearly on device screen
- [ ] UI elements are appropriately sized for touch
- [ ] Visual feedback for button presses
- [ ] Inventory grid interaction works smoothly

### **📊 Expected Test Results**

#### **Performance Benchmarks**
- **FPS**: Should maintain 60 FPS with 100+ entities
- **Memory**: Should stay within 1275 MB of 1536 MB budget
- **Load Time**: Game should start within 5 seconds
- **Battery**: 4+ hours of gameplay on typical device

#### **Compatibility Matrix**
| Device Type | Android Version | Expected Performance |
|-------------|----------------|---------------------|
| Retroid Pocket Flip 2 | Android 11+ | Optimal (60 FPS) |
| Modern Phone | Android 8.0+ | Excellent (60+ FPS) |
| Tablet | Android 8.0+ | Excellent (60+ FPS) |
| Emulator | Android 8.0+ | Good (30-60 FPS) |

### **🚀 Automated Testing**

#### **Unit Test Execution**
```bash
# Run C++ unit tests
cd ..  # Back to project root
cmake --build build
./build/engine/tests/d2_unit_tests

# Expected: 533+ tests passing
```

#### **Android Integration Tests**
```bash
cd android/
./gradlew connectedAndroidTest

# Expected: 19 Espresso tests passing
```

#### **Full Test Suite**
```bash
cd ..
./run_all_tests.sh

# Expected: 552 total tests (533 C++ + 19 Android)
```

### **📝 Test Report Template**

```
## Device Test Report

**Device**: [Device Name/Model]
**Android Version**: [Version]
**RAM**: [Amount]
**Processor**: [CPU Info]
**Controller**: [Controller Type if applicable]

### Performance Results
- Average FPS: ___
- Memory Usage: ___ MB
- Load Time: ___ seconds
- Battery Life: ___ hours

### Functionality Tests
- [ ] App Launch: Pass/Fail
- [ ] Menu Navigation: Pass/Fail  
- [ ] Character Movement: Pass/Fail
- [ ] Inventory Management: Pass/Fail
- [ ] Controller Input: Pass/Fail

### Issues Found
[Describe any crashes, performance issues, or UI problems]

### Screenshots/Videos
[Attach gameplay footage if possible]
```

### **🎯 Success Criteria**

The APK testing is considered successful when:

1. **✅ Stability**: App runs for 30+ minutes without crashes
2. **✅ Performance**: Maintains target FPS (60) during gameplay  
3. **✅ Controls**: Full controller navigation works smoothly
4. **✅ Memory**: Stays within memory budget during extended play
5. **✅ Compatibility**: Works on target Android versions (8.0+)

This testing framework validates that our **production-ready** implementation actually delivers the promised Diablo II experience on Android devices.