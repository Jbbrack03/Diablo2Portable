#pragma once

#include <cstdint>
#include <memory>

// Mock JNI types for desktop testing
using jlong = int64_t;
using jboolean = uint8_t;
using jint = int32_t;
using jfloat = float;
using jstring = void*;
using JNIEnv = void;
using jobject = void*;

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
    // Future: Add actual engine components here
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