#include "android/jni_bridge.h"

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#include <memory>

// Forward declarations for the engine
namespace d2 {
    class GameEngine;
}

// Global engine instance (in a real implementation this would be better managed)
static std::unique_ptr<d2::GameEngine> g_engine;

#define LOG_TAG "D2Portable"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

// Create the game engine instance
JNIEXPORT jlong JNICALL
Java_com_diablo2portable_NativeEngine_createEngine(JNIEnv *env, jobject thiz) {
    LOGI("Creating native game engine");
    
    try {
        // For now, return a placeholder value since GameEngine instantiation 
        // would require proper OpenGL context and asset loading
        return reinterpret_cast<jlong>(1);
    } catch (const std::exception& e) {
        LOGE("Failed to create engine: %s", e.what());
        return 0;
    }
}

// Initialize the game engine with rendering context
JNIEXPORT jboolean JNICALL
Java_com_diablo2portable_NativeEngine_initialize(JNIEnv *env, jobject thiz, 
                                                  jint width, jint height) {
    LOGI("Initializing engine with viewport %dx%d", width, height);
    
    try {
        // Initialize rendering system, input handlers, etc.
        // This would set up OpenGL context, load initial assets, etc.
        return JNI_TRUE;
    } catch (const std::exception& e) {
        LOGE("Failed to initialize engine: %s", e.what());
        return JNI_FALSE;
    }
}

// Render a single frame
JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_renderFrame(JNIEnv *env, jobject thiz) {
    // Render game frame
    // This would call the main game loop update and render methods
    
    // For now, just log that we're rendering (minimal implementation)
    static int frameCount = 0;
    if (frameCount++ % 60 == 0) {  // Log every 60 frames to avoid spam
        LOGI("Rendering frame %d", frameCount);
    }
}

// Handle touch input events
JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_onTouchEvent(JNIEnv *env, jobject thiz,
                                                    jfloat x, jfloat y, jint action) {
    // Process touch input
    // action: 0=down, 1=up, 2=move
    LOGI("Touch event: (%.2f, %.2f) action=%d", x, y, action);
}

// Load game assets
JNIEXPORT jboolean JNICALL
Java_com_diablo2portable_NativeEngine_loadAssets(JNIEnv *env, jobject thiz, jstring assetPath) {
    LOGI("Loading game assets");
    
    try {
        // Load MPQ files, extract sprites, initialize asset manager
        const char* pathStr = env->GetStringUTFChars(assetPath, nullptr);
        LOGI("Asset path: %s", pathStr);
        env->ReleaseStringUTFChars(assetPath, pathStr);
        
        return JNI_TRUE;
    } catch (const std::exception& e) {
        LOGE("Failed to load assets: %s", e.what());
        return JNI_FALSE;
    }
}

// Handle OpenGL surface creation
JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_onSurfaceCreated(JNIEnv *env, jobject thiz) {
    LOGI("OpenGL surface created");
    
    // Initialize OpenGL context, shaders, buffers
    // Set up rendering pipeline
}

// Cleanup and destroy engine
JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_destroyEngine(JNIEnv *env, jobject thiz) {
    LOGI("Destroying native game engine");
    
    // Cleanup resources
    g_engine.reset();
}

// Get engine status/info
JNIEXPORT jstring JNICALL
Java_com_diablo2portable_NativeEngine_getEngineInfo(JNIEnv *env, jobject thiz) {
    std::string info = "D2Portable Engine v1.0 - JNI Bridge Active";
    return env->NewStringUTF(info.c_str());
}

} // extern "C"

#else
// Desktop stub implementations for testing
extern "C" {

#include <cstdint>

// Use the same types as the header to avoid conflicts
#define JNI_TRUE 1
#define JNI_FALSE 0
#define JNIEXPORT

JNIEXPORT jlong
Java_com_diablo2portable_NativeEngine_createEngine(JNIEnv *env, jobject thiz) {
    return 1; // Stub implementation
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_destroyEngine(JNIEnv *env, jobject thiz, jlong handle) {
    // Stub implementation
}

JNIEXPORT jboolean
Java_com_diablo2portable_NativeEngine_initialize(JNIEnv *env, jobject thiz, jlong handle) {
    return JNI_TRUE; // Stub implementation
}

JNIEXPORT jboolean
Java_com_diablo2portable_NativeEngine_loadAssets(JNIEnv *env, jobject thiz, jlong handle, jstring assetPath) {
    return JNI_TRUE; // Stub implementation
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_onTouchEvent(JNIEnv *env, jobject thiz, jlong handle,
                                                    jfloat x, jfloat y, jint action) {
    // Stub implementation
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_onSurfaceCreated(JNIEnv *env, jobject thiz, jlong handle, jint width, jint height) {
    // Stub implementation
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_renderFrame(JNIEnv *env, jobject thiz, jlong handle) {
    // Stub implementation
}

JNIEXPORT jstring
Java_com_diablo2portable_NativeEngine_getEngineInfo(JNIEnv *env, jobject thiz) {
    return nullptr; // Stub implementation
}

} // extern "C"

#endif // __ANDROID__

// JNIBridge class implementation (cross-platform)
std::function<void(int, bool)> JNIBridge::gamepadCallback_;

bool JNIBridge::handleKeyEvent(int keyCode, int action) {
    printf("JNIBridge: handleKeyEvent(%d, %d)\n", keyCode, action);
    if (gamepadCallback_) {
        gamepadCallback_(keyCode, action == 1);
    }
    return true;
}

bool JNIBridge::handleMotionEvent(int axis1, float value1, int axis2, float value2, int source) {
    printf("JNIBridge: handleMotionEvent(%d=%.2f, %d=%.2f, source=%d)\n", 
           axis1, value1, axis2, value2, source);
    return true;
}

ControllerState JNIBridge::getControllerState() {
    ControllerState state;
    return state;
}

void JNIBridge::registerGamepadCallback(std::function<void(int, bool)> callback) {
    gamepadCallback_ = callback;
    printf("JNIBridge: Gamepad callback registered\n");
}

// GameEngineWrapper implementation
namespace d2::android {

GameEngineWrapper::GameEngineWrapper() 
    : initialized_(false), surface_width_(0), surface_height_(0), engine_(nullptr) {
    printf("GameEngineWrapper: Created\n");
}

GameEngineWrapper::~GameEngineWrapper() {
    printf("GameEngineWrapper: Destroyed\n");
}

bool GameEngineWrapper::initialize() {
    printf("GameEngineWrapper: Initialize\n");
    initialized_ = true;
    return true;
}

bool GameEngineWrapper::loadAssets(const char* asset_path) {
    printf("GameEngineWrapper: Load assets from %s\n", asset_path ? asset_path : "null");
    return true;
}

void GameEngineWrapper::onTouchEvent(float x, float y, int action) {
    printf("GameEngineWrapper: Touch event (%.2f, %.2f) action=%d\n", x, y, action);
}

void GameEngineWrapper::onSurfaceCreated(int width, int height) {
    printf("GameEngineWrapper: Surface created %dx%d\n", width, height);
    surface_width_ = width;
    surface_height_ = height;
}

void GameEngineWrapper::renderFrame() {
    static int frameCount = 0;
    if (frameCount++ % 60 == 0) {
        printf("GameEngineWrapper: Render frame %d\n", frameCount);
    }
}

} // namespace d2::android