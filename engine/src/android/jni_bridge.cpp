#include "android/jni_bridge.h"

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#include <memory>
#include "game/game_engine.h"
#include "rendering/render_context.h"
#include "rendering/gles3_render_backend.h"

#define LOG_TAG "D2Portable"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

struct AndroidEngineHandle {
    d2::rendering::GLES3RenderBackend backend;
    d2::GameEngine engine;
};

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_diablo2portable_NativeEngine_createEngine(JNIEnv *env, jobject thiz) {
    LOGI("Creating native game engine");
    try {
        auto* handle = new AndroidEngineHandle();
        d2::rendering::RenderContext::setBackend(&handle->backend);
        return reinterpret_cast<jlong>(handle);
    } catch (const std::exception& e) {
        LOGE("Failed to create engine: %s", e.what());
        return 0;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_diablo2portable_NativeEngine_initialize(JNIEnv *env, jobject thiz,
                                                  jint width, jint height) {
    LOGI("Initializing engine with viewport %dx%d", width, height);
    // Note: On Android the GL context is managed by GLSurfaceView.
    // The GLES3RenderBackend wraps real GL calls.
    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_renderFrame(JNIEnv *env, jobject thiz) {
    // Render will be called from the GL thread (GLSurfaceView.Renderer.onDrawFrame)
}

JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_onTouchEvent(JNIEnv *env, jobject thiz,
                                                    jfloat x, jfloat y, jint action) {
    LOGI("Touch event: (%.2f, %.2f) action=%d", x, y, action);
}

JNIEXPORT jboolean JNICALL
Java_com_diablo2portable_NativeEngine_loadAssets(JNIEnv *env, jobject thiz, jstring assetPath) {
    LOGI("Loading game assets");
    try {
        const char* pathStr = env->GetStringUTFChars(assetPath, nullptr);
        LOGI("Asset path: %s", pathStr);
        env->ReleaseStringUTFChars(assetPath, pathStr);
        return JNI_TRUE;
    } catch (const std::exception& e) {
        LOGE("Failed to load assets: %s", e.what());
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_onSurfaceCreated(JNIEnv *env, jobject thiz) {
    LOGI("OpenGL surface created - GL context now available");
}

JNIEXPORT void JNICALL
Java_com_diablo2portable_NativeEngine_destroyEngine(JNIEnv *env, jobject thiz) {
    LOGI("Destroying native game engine");
    // Note: handle-based destroy would need the handle parameter
    // For now this matches the existing Android JNI signature
}

JNIEXPORT jstring JNICALL
Java_com_diablo2portable_NativeEngine_getEngineInfo(JNIEnv *env, jobject thiz) {
    std::string info = "D2Portable Engine v2.0 - Real GameEngine + GLES3 Backend";
    return env->NewStringUTF(info.c_str());
}

} // extern "C"

#else
// Desktop implementations that create real GameEngine instances with MockRenderBackend
#include "game/game_engine.h"
#include "rendering/render_context.h"
#include "rendering/mock_render_backend.h"

extern "C" {

#include <cstdint>
#include <cstdio>

#define JNI_TRUE 1
#define JNI_FALSE 0
#define JNIEXPORT

// Desktop: each engine handle owns a GameEngine + MockRenderBackend pair
struct DesktopEngineHandle {
    d2::rendering::MockRenderBackend backend;
    d2::GameEngine engine;
};

JNIEXPORT jlong
Java_com_diablo2portable_NativeEngine_createEngine(JNIEnv *env, jobject thiz) {
    (void)env; (void)thiz;
    auto* handle = new DesktopEngineHandle();
    d2::rendering::RenderContext::setBackend(&handle->backend);
    return reinterpret_cast<jlong>(handle);
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_destroyEngine(JNIEnv *env, jobject thiz, jlong handle) {
    (void)env; (void)thiz;
    if (handle == 0) return;
    auto* h = reinterpret_cast<DesktopEngineHandle*>(handle);
    d2::rendering::RenderContext::setBackend(nullptr);
    delete h;
}

JNIEXPORT jboolean
Java_com_diablo2portable_NativeEngine_initialize(JNIEnv *env, jobject thiz, jlong handle) {
    (void)env; (void)thiz;
    if (handle == 0) return JNI_FALSE;
    auto* h = reinterpret_cast<DesktopEngineHandle*>(handle);
    return h->engine.initialize("", &h->backend) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean
Java_com_diablo2portable_NativeEngine_loadAssets(JNIEnv *env, jobject thiz, jlong handle, jstring assetPath) {
    (void)env; (void)thiz; (void)handle; (void)assetPath;
    return JNI_TRUE;
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_onTouchEvent(JNIEnv *env, jobject thiz, jlong handle,
                                                    jfloat x, jfloat y, jint action) {
    (void)env; (void)thiz; (void)handle; (void)x; (void)y; (void)action;
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_onSurfaceCreated(JNIEnv *env, jobject thiz, jlong handle, jint width, jint height) {
    (void)env; (void)thiz; (void)handle; (void)width; (void)height;
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_renderFrame(JNIEnv *env, jobject thiz, jlong handle) {
    (void)env; (void)thiz; (void)handle;
}

JNIEXPORT jstring
Java_com_diablo2portable_NativeEngine_getEngineInfo(JNIEnv *env, jobject thiz) {
    (void)env; (void)thiz;
    return nullptr;
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_onGamepadInput(JNIEnv *env, jobject thiz, jlong handle,
                                                      jfloat leftX, jfloat leftY,
                                                      jfloat rightX, jfloat rightY,
                                                      jfloat leftTrigger, jfloat rightTrigger) {
    (void)env; (void)thiz; (void)handle;
    (void)leftX; (void)leftY; (void)rightX; (void)rightY;
    (void)leftTrigger; (void)rightTrigger;
}

JNIEXPORT void
Java_com_diablo2portable_NativeEngine_onGamepadButton(JNIEnv *env, jobject thiz, jlong handle,
                                                       jint buttonCode, jboolean pressed) {
    (void)env; (void)thiz; (void)handle;
    (void)buttonCode; (void)pressed;
}

} // extern "C"

#endif // __ANDROID__

// JNIBridge class implementation (cross-platform)
std::function<void(int, bool)> JNIBridge::gamepadCallback_;
static ControllerState g_controllerState;

bool JNIBridge::handleKeyEvent(int keyCode, int action) {
    printf("JNIBridge: handleKeyEvent(%d, %d)\n", keyCode, action);
    
    // Map key codes to button indices
    int buttonIndex = -1;
    switch (keyCode) {
        case 96: // KEYCODE_BUTTON_A
            buttonIndex = 0;
            break;
        case 97: // KEYCODE_BUTTON_B
            buttonIndex = 1;
            break;
        case 98: // KEYCODE_BUTTON_X
            buttonIndex = 2;
            break;
        case 99: // KEYCODE_BUTTON_Y
            buttonIndex = 3;
            break;
        // Add more button mappings as needed
    }
    
    // Update button state if it's a known button
    if (buttonIndex >= 0 && buttonIndex < 16) {
        g_controllerState.buttons[buttonIndex] = (action == 0); // action 0 = pressed, 1 = released
    }
    
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
    return g_controllerState;
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