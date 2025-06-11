# Troubleshooting Guide - Diablo II Android Port

## Overview
This guide provides solutions to common issues encountered during development, building, and running the Diablo II Android port.

## Development Environment Issues

### 1. Android NDK Not Found

**Symptoms:**
```
CMake Error: CMAKE_ANDROID_NDK is not set
```

**Solution:**
```bash
# Set NDK path in local.properties
echo "ndk.dir=$HOME/Library/Android/sdk/ndk/25.2.9519653" >> local.properties

# Or set environment variable
export ANDROID_NDK=$HOME/Library/Android/sdk/ndk/25.2.9519653
export PATH=$PATH:$ANDROID_NDK

# Verify NDK installation
$ANDROID_NDK/ndk-build --version
```

### 2. CMake Version Mismatch

**Symptoms:**
```
CMake 3.22.1 or higher is required. You are running version 3.18.1
```

**Solution:**
```bash
# Install specific CMake version via SDK Manager
sdkmanager --install "cmake;3.22.1"

# Update gradle to use specific version
# In app/build.gradle:
externalNativeBuild {
    cmake {
        version "3.22.1"
        path "CMakeLists.txt"
    }
}
```

### 3. Missing Build Tools

**Symptoms:**
```
Failed to find Build Tools revision 33.0.0
```

**Solution:**
```bash
# Install required build tools
sdkmanager --install "build-tools;33.0.0"

# List all available build tools
sdkmanager --list | grep build-tools

# Install multiple versions if needed
sdkmanager --install "build-tools;33.0.0" "build-tools;32.0.0"
```

## Build Errors

### 1. C++ Compilation Errors

**Symptoms:**
```
error: no member named 'filesystem' in namespace 'std'
```

**Solution:**
```cmake
# In CMakeLists.txt, ensure C++17 is enabled:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For filesystem support on older NDK:
target_link_libraries(${PROJECT_NAME}
    android
    log
    c++_shared
)

# If still issues, use boost::filesystem instead
find_package(Boost REQUIRED COMPONENTS filesystem)
target_link_libraries(${PROJECT_NAME} Boost::filesystem)
```

### 2. Undefined Symbol Errors

**Symptoms:**
```
undefined reference to 'StormOpenArchive'
```

**Solution:**
```cmake
# Ensure StormLib is properly linked
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/StormLib)
target_link_libraries(${PROJECT_NAME}
    storm
    z  # zlib for compression
)

# Check library architecture matches
file libs/StormLib/libstorm.a
# Should show: archive (arm64)
```

### 3. Resource Processing Failed

**Symptoms:**
```
AAPT: error: resource drawable/icon not found
```

**Solution:**
```bash
# Clean and rebuild resources
./gradlew clean
rm -rf app/build/intermediates/res

# Verify resources exist
find app/src/main/res -name "*.png" -o -name "*.xml"

# Check for naming issues (no capitals, spaces)
# Rename if needed:
mv app/src/main/res/drawable/Icon.png app/src/main/res/drawable/icon.png
```

## Runtime Crashes

### 1. JNI DETECTED ERROR IN APPLICATION

**Symptoms:**
```
JNI DETECTED ERROR IN APPLICATION: use of deleted local reference
```

**Solution:**
```cpp
// Fix 1: Don't use JNI references after DeleteLocalRef
jobject obj = env->NewObject(...);
// Use obj
env->DeleteLocalRef(obj);
// Don't use obj after this!

// Fix 2: Use global references for long-term storage
class JNIHelper {
    jobject globalRef;
public:
    void init(JNIEnv* env, jobject localRef) {
        globalRef = env->NewGlobalRef(localRef);
    }
    ~JNIHelper() {
        if (globalRef) {
            env->DeleteGlobalRef(globalRef);
        }
    }
};

// Fix 3: Check for exceptions after JNI calls
env->CallVoidMethod(obj, methodID);
if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
    env->ExceptionClear();
    return; // Handle error
}
```

### 2. SIGSEGV (Signal 11)

**Symptoms:**
```
Fatal signal 11 (SIGSEGV), code 1, fault addr 0x0
```

**Solution:**
```cpp
// Enable debugging symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

// Use address sanitizer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

// Add null checks
if (ptr != nullptr) {
    ptr->doSomething();
}

// Use smart pointers
std::unique_ptr<Object> obj = std::make_unique<Object>();

// Debug with ndk-stack
adb logcat | ndk-stack -sym app/build/intermediates/cmake/debug/obj/arm64-v8a
```

### 3. OpenGL ES Errors

**Symptoms:**
```
E/libEGL: call to OpenGL ES API with no current context
```

**Solution:**
```cpp
// Ensure context is current before GL calls
class Renderer {
    EGLContext context;
    EGLSurface surface;
    
    void makeCurrent() {
        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
            LOGE("Failed to make context current: %d", eglGetError());
        }
    }
    
    void render() {
        makeCurrent();  // Always ensure context
        
        // Now safe to make GL calls
        glClear(GL_COLOR_BUFFER_BIT);
        // ... rest of rendering
    }
};

// Check for GL errors
void checkGLError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("GL Error after %s: 0x%x", op, error);
    }
}
```

## Asset Loading Issues

### 1. MPQ Files Not Found

**Symptoms:**
```
Failed to open MPQ archive: d2data.mpq
```

**Solution:**
```cpp
// Use proper asset paths on Android
std::string getAssetPath(const std::string& filename) {
    #ifdef __ANDROID__
        // Assets are in APK, need to extract first
        return extractAssetToCache(filename);
    #else
        return "assets/" + filename;
    #endif
}

// Extract from APK to cache
std::string extractAssetToCache(const std::string& assetPath) {
    AAssetManager* mgr = androidApp->activity->assetManager;
    AAsset* asset = AAssetManager_open(mgr, assetPath.c_str(), AASSET_MODE_BUFFER);
    
    if (!asset) return "";
    
    size_t size = AAsset_getLength(asset);
    std::vector<char> buffer(size);
    AAsset_read(asset, buffer.data(), size);
    AAsset_close(asset);
    
    // Write to cache
    std::string cachePath = getCacheDir() + "/" + assetPath;
    std::ofstream out(cachePath, std::ios::binary);
    out.write(buffer.data(), size);
    
    return cachePath;
}
```

### 2. Sprite Loading Failures

**Symptoms:**
```
Failed to decode DC6 sprite: invalid header
```

**Solution:**
```bash
# Verify file integrity
md5sum original/d2data.mpq
# Compare with known good hash

# Test extraction
./tools/mpq_test d2data.mpq "data/global/chars/BA/BA.dc6"

# Check file format
hexdump -C sprite.dc6 | head -20
# Should show DC6 header: 06 00 00 00 01 00 00 00
```

### 3. Out of Memory

**Symptoms:**
```
java.lang.OutOfMemoryError: Failed to allocate
```

**Solution:**
```java
// In AndroidManifest.xml:
<application
    android:largeHeap="true"
    android:hardwareAccelerated="true">

// Implement asset streaming
class AssetStreamer {
    private static final int MAX_CACHE_SIZE = 100 * 1024 * 1024; // 100MB
    private LruCache<String, Sprite> spriteCache;
    
    public AssetStreamer() {
        spriteCache = new LruCache<String, Sprite>(MAX_CACHE_SIZE) {
            @Override
            protected int sizeOf(String key, Sprite sprite) {
                return sprite.getMemorySize();
            }
        };
    }
}
```

## Controller Issues

### 1. Controller Not Detected

**Symptoms:**
```
No gamepad found
```

**Solution:**
```java
// Check for input device capabilities
public boolean isGamepad(InputDevice device) {
    int sources = device.getSources();
    return ((sources & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD)
        || ((sources & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK);
}

// List all input devices
int[] deviceIds = InputDevice.getDeviceIds();
for (int deviceId : deviceIds) {
    InputDevice device = InputDevice.getDevice(deviceId);
    if (isGamepad(device)) {
        Log.d(TAG, "Found gamepad: " + device.getName());
    }
}

// Handle generic controllers
@Override
public boolean onGenericMotionEvent(MotionEvent event) {
    if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) != 0) {
        // Process joystick input
        float x = event.getAxisValue(MotionEvent.AXIS_X);
        float y = event.getAxisValue(MotionEvent.AXIS_Y);
        // ...
        return true;
    }
    return super.onGenericMotionEvent(event);
}
```

### 2. Button Mapping Issues

**Symptoms:**
```
Controller buttons not working correctly
```

**Solution:**
```java
// Create controller profile system
public class ControllerProfile {
    private Map<Integer, Integer> buttonMap = new HashMap<>();
    
    public void loadProfile(String controllerName) {
        if (controllerName.contains("Xbox")) {
            buttonMap.put(KeyEvent.KEYCODE_BUTTON_A, GameButton.ACTION);
            buttonMap.put(KeyEvent.KEYCODE_BUTTON_B, GameButton.CANCEL);
        } else if (controllerName.contains("PlayStation")) {
            buttonMap.put(KeyEvent.KEYCODE_BUTTON_B, GameButton.ACTION);  // Circle
            buttonMap.put(KeyEvent.KEYCODE_BUTTON_C, GameButton.CANCEL); // X
        }
    }
}

// Debug button codes
@Override
public boolean onKeyDown(int keyCode, KeyEvent event) {
    Log.d(TAG, "Button pressed: " + keyCode + " (" + KeyEvent.keyCodeToString(keyCode) + ")");
    return super.onKeyDown(keyCode, event);
}
```

## Performance Issues

### 1. Low Frame Rate

**Symptoms:**
```
FPS dropping below 30
```

**Solution:**
```cpp
// Profile with systrace
class PerformanceTracer {
    void beginFrame() {
        ATrace_beginSection("GameFrame");
    }
    
    void beginSection(const char* name) {
        ATrace_beginSection(name);
    }
    
    void endSection() {
        ATrace_endSection();
    }
};

// Optimize render calls
class BatchRenderer {
    static constexpr int MAX_SPRITES_PER_BATCH = 1000;
    
    void flushIfNeeded() {
        if (spriteCount >= MAX_SPRITES_PER_BATCH) {
            flush();
        }
    }
};

// Use texture atlases
class TextureAtlas {
    void packTextures() {
        // Combine small textures into larger atlases
        // Reduces texture switches
    }
};
```

### 2. Memory Leaks

**Symptoms:**
```
Memory usage continuously increasing
```

**Solution:**
```bash
# Enable LeakCanary in debug builds
dependencies {
    debugImplementation 'com.squareup.leakcanary:leakcanary-android:2.10'
}

# Use Android Studio memory profiler
# View > Tool Windows > Profiler

# Native memory debugging
adb shell am dumpheap com.personal.diablo2portable /data/local/tmp/heap.hprof
adb pull /data/local/tmp/heap.hprof
# Analyze with Android Studio
```

### 3. Battery Drain

**Symptoms:**
```
Excessive battery consumption
```

**Solution:**
```cpp
// Implement frame rate limiting
class FrameLimiter {
    static constexpr int TARGET_FPS = 60;
    static constexpr auto FRAME_TIME = std::chrono::milliseconds(1000 / TARGET_FPS);
    
    void limitFrameRate() {
        auto frameEnd = std::chrono::steady_clock::now();
        auto frameDuration = frameEnd - frameStart;
        
        if (frameDuration < FRAME_TIME) {
            std::this_thread::sleep_for(FRAME_TIME - frameDuration);
        }
        
        frameStart = std::chrono::steady_clock::now();
    }
};

// Reduce GPU usage when possible
if (isInMenu || isPaused) {
    setFrameRateLimit(30);  // Lower FPS in menus
}
```

## Network Issues

### 1. LAN Game Discovery Failed

**Symptoms:**
```
No games found on local network
```

**Solution:**
```java
// Request necessary permissions
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
<uses-permission android:name="android.permission.CHANGE_WIFI_MULTICAST_STATE" />

// Enable multicast
WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
WifiManager.MulticastLock lock = wifi.createMulticastLock("D2Multicast");
lock.acquire();

// Use broadcast address
try {
    DatagramSocket socket = new DatagramSocket();
    socket.setBroadcast(true);
    
    byte[] sendData = "D2GAME".getBytes();
    DatagramPacket packet = new DatagramPacket(
        sendData, 
        sendData.length,
        InetAddress.getByName("255.255.255.255"), 
        6112
    );
    socket.send(packet);
} catch (Exception e) {
    Log.e(TAG, "Broadcast failed", e);
}
```

### 2. Connection Timeout

**Symptoms:**
```
Failed to join game: timeout
```

**Solution:**
```cpp
// Increase timeout values
socket.setSoTimeout(10000);  // 10 seconds

// Add retry logic
bool connectWithRetry(const std::string& host, int port, int maxRetries = 3) {
    for (int i = 0; i < maxRetries; i++) {
        if (connect(host, port)) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return false;
}

// Check firewall settings
// Some Android devices block game ports
```

## Debug Tools

### 1. Enable Verbose Logging

```cpp
// In debug builds
#ifdef DEBUG
    #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "D2Port", __VA_ARGS__)
#else
    #define LOGD(...) ((void)0)
#endif

// Use logcat filters
adb logcat -s D2Port:V
```

### 2. GPU Debugging

```bash
# Enable GPU profiling
adb shell setprop debug.hwui.overdraw show
adb shell setprop debug.hwui.profile true

# Capture GPU trace
adb shell atrace --async_start -a com.personal.diablo2portable gfx
# ... reproduce issue ...
adb shell atrace --async_stop -o /sdcard/trace.txt
adb pull /sdcard/trace.txt
```

### 3. Network Debugging

```bash
# Monitor network traffic
adb shell tcpdump -i any -w /sdcard/capture.pcap
adb pull /sdcard/capture.pcap
# Analyze with Wireshark

# Check port binding
adb shell netstat -an | grep 6112
```

## Common Error Messages

| Error | Cause | Solution |
|-------|-------|----------|
| `INSTALL_FAILED_NO_MATCHING_ABIS` | Wrong architecture | Build for device ABI |
| `java.lang.UnsatisfiedLinkError` | Native library not found | Check library name and path |
| `EGL_BAD_CONFIG` | Invalid GL configuration | Verify device GL support |
| `SIGABRT` | Assertion failed | Check debug logs |
| `ANR in com.personal.diablo2portable` | UI thread blocked | Move work to background |

This troubleshooting guide covers the most common issues. For additional help, check the logs and use the debug tools provided.