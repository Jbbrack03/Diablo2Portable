#include "android/jni_bridge.h"
#include <cassert>
#include <cstring>

namespace d2::android {

GameEngineWrapper::GameEngineWrapper() 
    : initialized_(false), surface_width_(0), surface_height_(0) {
}

GameEngineWrapper::~GameEngineWrapper() {
    // Cleanup resources
}

bool GameEngineWrapper::initialize() {
    if (initialized_) {
        return true;
    }
    
    // Initialize engine components
    // For now, just mark as initialized
    initialized_ = true;
    return true;
}

bool GameEngineWrapper::loadAssets(const char* asset_path) {
    if (!initialized_) {
        return false;
    }
    
    if (!asset_path) {
        return false;
    }
    
    // TODO: Integrate with AssetManager
    // For now, just validate the path is not empty
    return strlen(asset_path) > 0;
}

void GameEngineWrapper::onTouchEvent(float x, float y, int action) {
    if (!initialized_) {
        return;
    }
    
    // TODO: Process touch input
    // For now, just store the coordinates (unused but validates parameters)
    (void)x;
    (void)y;
    (void)action;
}

void GameEngineWrapper::onSurfaceCreated(int width, int height) {
    if (!initialized_) {
        return;
    }
    
    if (width <= 0 || height <= 0) {
        return;
    }
    
    surface_width_ = width;
    surface_height_ = height;
    
    // TODO: Initialize renderer with surface dimensions
}

void GameEngineWrapper::renderFrame() {
    if (!initialized_) {
        return;
    }
    
    if (surface_width_ <= 0 || surface_height_ <= 0) {
        return;
    }
    
    // TODO: Render game frame
    // For now, just verify we have valid surface dimensions
}

} // namespace d2::android

// JNI Bridge Implementation
extern "C" {

jlong Java_com_diablo2portable_NativeEngine_createEngine(JNIEnv* env, jobject obj) {
    (void)env;
    (void)obj;
    
    try {
        auto* engine = new d2::android::GameEngineWrapper();
        return reinterpret_cast<jlong>(engine);
    } catch (...) {
        return 0;
    }
}

void Java_com_diablo2portable_NativeEngine_destroyEngine(JNIEnv* env, jobject obj, jlong handle) {
    (void)env;
    (void)obj;
    
    if (handle == 0) {
        return;
    }
    
    auto* engine = reinterpret_cast<d2::android::GameEngineWrapper*>(handle);
    delete engine;
}

jboolean Java_com_diablo2portable_NativeEngine_initialize(JNIEnv* env, jobject obj, jlong handle) {
    (void)env;
    (void)obj;
    
    if (handle == 0) {
        return false;
    }
    
    auto* engine = reinterpret_cast<d2::android::GameEngineWrapper*>(handle);
    return engine->initialize() ? 1 : 0;
}

jboolean Java_com_diablo2portable_NativeEngine_loadAssets(JNIEnv* env, jobject obj, jlong handle, jstring asset_path) {
    (void)env;
    (void)obj;
    
    if (handle == 0 || !asset_path) {
        return false;
    }
    
    auto* engine = reinterpret_cast<d2::android::GameEngineWrapper*>(handle);
    
    // In real implementation, would convert jstring to const char*
    // For testing, we just cast the mock jstring
    const char* path = static_cast<const char*>(asset_path);
    
    return engine->loadAssets(path) ? 1 : 0;
}

void Java_com_diablo2portable_NativeEngine_onTouchEvent(JNIEnv* env, jobject obj, jlong handle, jfloat x, jfloat y, jint action) {
    (void)env;
    (void)obj;
    
    if (handle == 0) {
        return;
    }
    
    auto* engine = reinterpret_cast<d2::android::GameEngineWrapper*>(handle);
    engine->onTouchEvent(x, y, action);
}

void Java_com_diablo2portable_NativeEngine_onSurfaceCreated(JNIEnv* env, jobject obj, jlong handle, jint width, jint height) {
    (void)env;
    (void)obj;
    
    if (handle == 0) {
        return;
    }
    
    auto* engine = reinterpret_cast<d2::android::GameEngineWrapper*>(handle);
    engine->onSurfaceCreated(width, height);
}

void Java_com_diablo2portable_NativeEngine_renderFrame(JNIEnv* env, jobject obj, jlong handle) {
    (void)env;
    (void)obj;
    
    if (handle == 0) {
        return;
    }
    
    auto* engine = reinterpret_cast<d2::android::GameEngineWrapper*>(handle);
    engine->renderFrame();
}

}