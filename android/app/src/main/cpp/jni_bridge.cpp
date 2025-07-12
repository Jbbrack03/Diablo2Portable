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