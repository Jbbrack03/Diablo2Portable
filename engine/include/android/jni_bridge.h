#pragma once

#include <cstdint>
#include <memory>
#include <functional>

// Mock JNI types for desktop testing
using jlong = int64_t;
using jboolean = uint8_t;
using jint = int32_t;
using jfloat = float;
using jstring = void*;
using JNIEnv = void;
using jobject = void*;

// Controller state structure for UI queries
struct ControllerState {
    bool buttons[16] = {false};
    float axes[6] = {0.0f};
};

// Static JNI Bridge interface for gamepad handling
class JNIBridge {
public:
    static bool handleKeyEvent(int keyCode, int action);
    static bool handleMotionEvent(int axis1, float value1, int axis2, float value2, int source);
    static ControllerState getControllerState();
    static void registerGamepadCallback(std::function<void(int, bool)> callback);
private:
    static std::function<void(int, bool)> gamepadCallback_;
};

namespace d2 {
class GameEngine;
}

namespace d2::android {

/**
 * Game Engine wrapper for JNI interface
 * Manages the lifecycle and state of the native game engine
 */
class GameEngineWrapper {
public:
    GameEngineWrapper();
    ~GameEngineWrapper();
    
    bool initialize();
    bool loadAssets(const char* asset_path);
    void onTouchEvent(float x, float y, int action);
    void onSurfaceCreated(int width, int height);
    void renderFrame();
    
    bool isInitialized() const { return initialized_; }
    
private:
    bool initialized_;
    int surface_width_;
    int surface_height_;
    std::unique_ptr<d2::GameEngine> engine_;
};

// JNI Bridge Functions
extern "C" {
    jlong Java_com_diablo2portable_NativeEngine_createEngine(JNIEnv* env, jobject obj);
    void Java_com_diablo2portable_NativeEngine_destroyEngine(JNIEnv* env, jobject obj, jlong handle);
    jboolean Java_com_diablo2portable_NativeEngine_initialize(JNIEnv* env, jobject obj, jlong handle);
    jboolean Java_com_diablo2portable_NativeEngine_loadAssets(JNIEnv* env, jobject obj, jlong handle, jstring asset_path);
    void Java_com_diablo2portable_NativeEngine_onTouchEvent(JNIEnv* env, jobject obj, jlong handle, jfloat x, jfloat y, jint action);
    void Java_com_diablo2portable_NativeEngine_onSurfaceCreated(JNIEnv* env, jobject obj, jlong handle, jint width, jint height);
    void Java_com_diablo2portable_NativeEngine_renderFrame(JNIEnv* env, jobject obj, jlong handle);
}

} // namespace d2::android