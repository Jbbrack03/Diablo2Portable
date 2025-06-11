# Deployment Guide - Diablo II Android Port

## Overview
This guide covers the complete process of building, signing, and deploying the Diablo II Android port APK for personal use.

## Prerequisites

### Development Environment
- Android Studio configured
- Android SDK (API 26+)
- Android NDK (r25c or newer)
- CMake 3.22+
- Valid keystore for signing

### Game Assets
- Legally owned Diablo II files extracted
- Assets processed and optimized
- All tests passing

## Build Configuration

### 1. Configure Build Variants

#### app/build.gradle
```gradle
android {
    compileSdk 33
    
    defaultConfig {
        applicationId "com.personal.diablo2portable"
        minSdk 26
        targetSdk 33
        versionCode 1
        versionName "1.0.0"
        
        testInstrumentationRunner "androidx.test.runner.AndroidJUnitRunner"
        
        externalNativeBuild {
            cmake {
                cppFlags "-std=c++17 -frtti -fexceptions"
                arguments "-DANDROID_STL=c++_shared",
                         "-DANDROID_ARM_NEON=TRUE"
            }
        }
        
        ndk {
            abiFilters 'arm64-v8a', 'armeabi-v7a'
        }
    }
    
    buildTypes {
        debug {
            debuggable true
            minifyEnabled false
            ndk {
                debugSymbolLevel 'FULL'
            }
        }
        
        release {
            debuggable false
            minifyEnabled true
            shrinkResources true
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 
                         'proguard-rules.pro'
            ndk {
                debugSymbolLevel 'SYMBOL_TABLE'
            }
        }
        
        profile {
            initWith release
            debuggable true
            minifyEnabled false
            applicationIdSuffix ".profile"
        }
    }
    
    bundle {
        language {
            enableSplit = false
        }
        density {
            enableSplit = false
        }
        abi {
            enableSplit = true
        }
    }
}
```

### 2. ProGuard Configuration

#### proguard-rules.pro
```proguard
# Preserve game engine classes
-keep class com.d2portable.engine.** { *; }

# Preserve JNI methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# Preserve custom views
-keep public class * extends android.view.View {
    *** get*();
    void set*(***);
    public <init>(android.content.Context);
    public <init>(android.content.Context, android.util.AttributeSet);
    public <init>(android.content.Context, android.util.AttributeSet, int);
}

# OpenGL ES
-keep class javax.microedition.khronos.** { *; }
-keep class android.opengl.** { *; }

# Game save serialization
-keepclassmembers class com.d2portable.save.** {
    <fields>;
    <init>();
}

# Remove logging in release
-assumenosideeffects class android.util.Log {
    public static boolean isLoggable(java.lang.String, int);
    public static int d(...);
    public static int v(...);
    public static int i(...);
}
```

## Signing Configuration

### 1. Generate Release Keystore

```bash
# Generate new keystore for release signing
keytool -genkey -v -keystore release-keystore.jks \
    -keyalg RSA -keysize 4096 \
    -validity 10000 \
    -alias diablo2portable

# Verify keystore
keytool -list -v -keystore release-keystore.jks
```

### 2. Configure Signing

#### keystore.properties (git-ignored)
```properties
storeFile=../release-keystore.jks
storePassword=your_store_password
keyAlias=diablo2portable
keyPassword=your_key_password
```

#### app/build.gradle
```gradle
def keystorePropertiesFile = rootProject.file("keystore.properties")
def keystoreProperties = new Properties()
if (keystorePropertiesFile.exists()) {
    keystoreProperties.load(new FileInputStream(keystorePropertiesFile))
}

android {
    signingConfigs {
        release {
            storeFile file(keystoreProperties['storeFile'] ?: "dummy")
            storePassword keystoreProperties['storePassword'] ?: ""
            keyAlias keystoreProperties['keyAlias'] ?: ""
            keyPassword keystoreProperties['keyPassword'] ?: ""
        }
    }
    
    buildTypes {
        release {
            signingConfig signingConfigs.release
        }
    }
}
```

## Build Process

### 1. Clean Build Environment

```bash
#!/bin/bash
# clean_build.sh

echo "Cleaning build environment..."

# Clean gradle cache
./gradlew clean

# Clean CMake cache
rm -rf app/.cxx
rm -rf build

# Clean old APKs
rm -rf app/build/outputs/apk

echo "Build environment cleaned!"
```

### 2. Prepare Assets

```bash
#!/bin/bash
# prepare_assets.sh

echo "Preparing game assets..."

# Ensure assets are extracted
if [ ! -d "app/src/main/assets/data" ]; then
    echo "Extracting game assets..."
    ./tools/extract_assets.sh "$D2_GAME_PATH" "app/src/main/assets"
fi

# Optimize assets for mobile
echo "Optimizing assets..."
./tools/optimize_assets.sh "app/src/main/assets"

# Verify asset integrity
echo "Verifying assets..."
./tools/verify_assets.sh "app/src/main/assets" || exit 1

echo "Assets ready!"
```

### 3. Build APK

#### Debug Build
```bash
#!/bin/bash
# build_debug.sh

echo "Building debug APK..."

# Set up environment
export ANDROID_HOME=${ANDROID_HOME:-"$HOME/Library/Android/sdk"}
export PATH=$PATH:$ANDROID_HOME/platform-tools

# Build debug APK
./gradlew assembleDebug

# Check build result
if [ $? -eq 0 ]; then
    echo "Debug APK built successfully!"
    echo "Location: app/build/outputs/apk/debug/app-debug.apk"
else
    echo "Build failed!"
    exit 1
fi
```

#### Release Build
```bash
#!/bin/bash
# build_release.sh

echo "Building release APK..."

# Ensure keystore exists
if [ ! -f "release-keystore.jks" ]; then
    echo "Error: release-keystore.jks not found!"
    echo "Run: keytool -genkey -v -keystore release-keystore.jks -keyalg RSA -keysize 4096 -validity 10000 -alias diablo2portable"
    exit 1
fi

# Build release APK
./gradlew assembleRelease

# Verify APK is signed
jarsigner -verify -verbose -certs \
    app/build/outputs/apk/release/app-release.apk

if [ $? -eq 0 ]; then
    echo "Release APK built and signed successfully!"
    echo "Location: app/build/outputs/apk/release/app-release.apk"
else
    echo "Build or signing failed!"
    exit 1
fi
```

## Optimization

### 1. APK Size Reduction

```bash
#!/bin/bash
# optimize_apk.sh

INPUT_APK="app/build/outputs/apk/release/app-release.apk"
OUTPUT_APK="app/build/outputs/apk/release/app-release-optimized.apk"

# Analyze APK size
echo "Analyzing APK..."
$ANDROID_HOME/cmdline-tools/latest/bin/apkanalyzer apk summary $INPUT_APK

# Optimize with zipalign
echo "Optimizing alignment..."
$ANDROID_HOME/build-tools/33.0.0/zipalign -v -f 4 $INPUT_APK $OUTPUT_APK

# Verify optimization
$ANDROID_HOME/build-tools/33.0.0/zipalign -c -v 4 $OUTPUT_APK

echo "APK optimized!"
```

### 2. Split APKs by ABI

```gradle
// app/build.gradle
android {
    splits {
        abi {
            enable true
            reset()
            include 'arm64-v8a', 'armeabi-v7a'
            universalApk false
        }
    }
}

// Version code scheme
ext.abiCodes = ['armeabi-v7a': 1, 'arm64-v8a': 2]

android.applicationVariants.all { variant ->
    variant.outputs.each { output ->
        def baseAbiVersionCode = project.ext.abiCodes.get(
            output.getFilter(OutputFile.ABI)
        )
        if (baseAbiVersionCode != null) {
            output.versionCodeOverride =
                baseAbiVersionCode * 1000 + variant.versionCode
        }
    }
}
```

## Deployment

### 1. Local Device Installation

```bash
#!/bin/bash
# deploy_to_device.sh

APK_PATH="app/build/outputs/apk/release/app-release.apk"

echo "Checking for connected devices..."
adb devices

# Uninstall old version
echo "Uninstalling old version..."
adb uninstall com.personal.diablo2portable 2>/dev/null

# Install new APK
echo "Installing APK..."
adb install -r $APK_PATH

if [ $? -eq 0 ]; then
    echo "Installation successful!"
    
    # Launch app
    echo "Launching app..."
    adb shell am start -n com.personal.diablo2portable/.MainActivity
else
    echo "Installation failed!"
    exit 1
fi
```

### 2. Retroid Pocket Flip 2 Deployment

```bash
#!/bin/bash
# deploy_to_retroid.sh

# Enable ADB over WiFi on Retroid first
RETROID_IP="192.168.1.100"  # Replace with actual IP

echo "Connecting to Retroid Pocket Flip 2..."
adb connect $RETROID_IP:5555

# Wait for connection
sleep 2

# Deploy
./deploy_to_device.sh

# Optimize for device
echo "Configuring for Retroid..."
adb shell settings put global window_animation_scale 0.5
adb shell settings put global transition_animation_scale 0.5
adb shell settings put global animator_duration_scale 0.5
```

### 3. APK Distribution

#### Create Distribution Package
```bash
#!/bin/bash
# create_distribution.sh

VERSION="1.0.0"
DATE=$(date +%Y%m%d)
DIST_DIR="dist/Diablo2Portable_${VERSION}_${DATE}"

mkdir -p $DIST_DIR

# Copy APKs
cp app/build/outputs/apk/release/*.apk $DIST_DIR/

# Create README
cat > $DIST_DIR/README.txt << EOF
Diablo II Portable for Android
Version: $VERSION
Build Date: $DATE

Installation Instructions:
1. Enable "Unknown Sources" in Android Settings
2. Copy APK to device
3. Install APK using file manager
4. Ensure game files are in /sdcard/Diablo2Portable/

Requirements:
- Android 8.0 or higher
- 2GB RAM minimum
- 4GB free storage
- Gamepad recommended

APK Files:
- app-release.apk: Universal build (larger size)
- app-arm64-v8a-release.apk: 64-bit ARM devices
- app-armeabi-v7a-release.apk: 32-bit ARM devices

Note: This requires legally owned Diablo II game files.
EOF

# Create installation script
cat > $DIST_DIR/install.sh << 'EOF'
#!/bin/bash
echo "Diablo II Portable Installer"
echo "=========================="

# Detect device architecture
ABI=$(adb shell getprop ro.product.cpu.abi)
echo "Device architecture: $ABI"

# Select appropriate APK
if [[ $ABI == *"arm64"* ]]; then
    APK="app-arm64-v8a-release.apk"
elif [[ $ABI == *"armeabi"* ]]; then
    APK="app-armeabi-v7a-release.apk"
else
    APK="app-release.apk"
fi

echo "Installing $APK..."
adb install -r $APK
EOF

chmod +x $DIST_DIR/install.sh

# Create ZIP
cd dist
zip -r "Diablo2Portable_${VERSION}_${DATE}.zip" "Diablo2Portable_${VERSION}_${DATE}"

echo "Distribution package created: dist/Diablo2Portable_${VERSION}_${DATE}.zip"
```

## Post-Deployment

### 1. Verification Tests

```bash
#!/bin/bash
# verify_deployment.sh

PACKAGE="com.personal.diablo2portable"

echo "Verifying deployment..."

# Check if installed
if adb shell pm list packages | grep -q $PACKAGE; then
    echo "✓ App installed"
else
    echo "✗ App not found"
    exit 1
fi

# Check version
VERSION=$(adb shell dumpsys package $PACKAGE | grep versionName | head -1 | cut -d'=' -f2)
echo "✓ Version: $VERSION"

# Check permissions
echo "✓ Permissions:"
adb shell dumpsys package $PACKAGE | grep permission

# Launch and check for crashes
echo "Testing launch..."
adb shell am start -n $PACKAGE/.MainActivity
sleep 5

# Check if running
if adb shell ps | grep -q $PACKAGE; then
    echo "✓ App running"
else
    echo "✗ App crashed"
    exit 1
fi
```

### 2. Performance Monitoring

```bash
#!/bin/bash
# monitor_performance.sh

PACKAGE="com.personal.diablo2portable"

# Start performance monitoring
echo "Starting performance monitor..."

# CPU usage
adb shell top -n 1 | grep $PACKAGE

# Memory usage
adb shell dumpsys meminfo $PACKAGE | grep "TOTAL"

# GPU profiling
adb shell setprop debug.hwui.profile true
adb shell setprop debug.hwui.profile.file /sdcard/profile.txt

# Battery usage
adb shell dumpsys batterystats --charged $PACKAGE
```

## Troubleshooting

### Common Issues

#### 1. Build Failures
```bash
# Clean everything
./gradlew clean
rm -rf ~/.gradle/caches/
./gradlew --stop

# Rebuild with info
./gradlew assembleRelease --info
```

#### 2. Signing Issues
```bash
# Verify keystore
keytool -list -v -keystore release-keystore.jks

# Check APK signature
jarsigner -verify -verbose app-release.apk

# Re-sign if needed
jarsigner -verbose -sigalg SHA256withRSA -digestalg SHA-256 \
    -keystore release-keystore.jks app-release.apk diablo2portable
```

#### 3. Installation Failures
```bash
# Check device storage
adb shell df -h

# Clear package data
adb shell pm clear com.personal.diablo2portable

# Install with verbose logging
adb install -r -d app-release.apk
```

## Security Considerations

### 1. Code Obfuscation
- ProGuard enabled for release builds
- Native code symbols stripped
- Debug logging removed

### 2. Asset Protection
- Assets compressed and encrypted
- Integrity checks on game files
- No network features that could leak data

### 3. Safe Distribution
- APK signed with private key
- No automatic updates
- No analytics or tracking

## Final Checklist

Before deployment, ensure:

- [ ] All tests passing
- [ ] Release build successful
- [ ] APK properly signed
- [ ] APK size optimized
- [ ] Tested on target device
- [ ] Performance acceptable
- [ ] No crashes in 1-hour play session
- [ ] Documentation updated
- [ ] Version number incremented
- [ ] Git tag created

This deployment guide ensures a smooth process from build to installation on personal devices.