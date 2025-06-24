#include "android/jni_bridge.h"
#include "android/gamepad_handler.h"
#include "game/game_engine.h"
#include "input/touch_input.h"
#include <cassert>
#include <cstring>
#include <memory>

// Static members for JNIBridge
std::function<void(int, bool)> JNIBridge::gamepadCallback_ = nullptr;
static std::unique_ptr<GamepadHandler> g_gamepadHandler = std::make_unique<GamepadHandler>();

bool JNIBridge::handleKeyEvent(int keyCode, int action) {
    bool handled = g_gamepadHandler->handleKeyEvent(keyCode, action);
    
    if (handled && gamepadCallback_) {
        bool pressed = (action == 0); // ACTION_DOWN
        gamepadCallback_(keyCode, pressed);
    }
    
    return handled;
}

bool JNIBridge::handleMotionEvent(int axis1, float value1, int axis2, float value2, int source) {
    return g_gamepadHandler->handleMotionEvent(axis1, value1, axis2, value2, source);
}

ControllerState JNIBridge::getControllerState() {
    ControllerState state;
    
    // Map gamepad buttons to state array
    state.buttons[0] = g_gamepadHandler->isButtonPressed(GamepadButton::A);
    state.buttons[1] = g_gamepadHandler->isButtonPressed(GamepadButton::B);
    state.buttons[2] = g_gamepadHandler->isButtonPressed(GamepadButton::X);
    state.buttons[3] = g_gamepadHandler->isButtonPressed(GamepadButton::Y);
    state.buttons[4] = g_gamepadHandler->isButtonPressed(GamepadButton::LEFT_BUMPER);
    state.buttons[5] = g_gamepadHandler->isButtonPressed(GamepadButton::RIGHT_BUMPER);
    state.buttons[6] = g_gamepadHandler->isButtonPressed(GamepadButton::LEFT_TRIGGER);
    state.buttons[7] = g_gamepadHandler->isButtonPressed(GamepadButton::RIGHT_TRIGGER);
    state.buttons[8] = g_gamepadHandler->isButtonPressed(GamepadButton::START);
    state.buttons[9] = g_gamepadHandler->isButtonPressed(GamepadButton::SELECT);
    state.buttons[10] = g_gamepadHandler->isButtonPressed(GamepadButton::DPAD_UP);
    state.buttons[11] = g_gamepadHandler->isButtonPressed(GamepadButton::DPAD_DOWN);
    state.buttons[12] = g_gamepadHandler->isButtonPressed(GamepadButton::DPAD_LEFT);
    state.buttons[13] = g_gamepadHandler->isButtonPressed(GamepadButton::DPAD_RIGHT);
    
    // Map axes
    state.axes[0] = g_gamepadHandler->getAxisValue(GamepadAxis::LEFT_X);
    state.axes[1] = g_gamepadHandler->getAxisValue(GamepadAxis::LEFT_Y);
    state.axes[2] = g_gamepadHandler->getAxisValue(GamepadAxis::RIGHT_X);
    state.axes[3] = g_gamepadHandler->getAxisValue(GamepadAxis::RIGHT_Y);
    state.axes[4] = g_gamepadHandler->getAxisValue(GamepadAxis::LEFT_TRIGGER);
    state.axes[5] = g_gamepadHandler->getAxisValue(GamepadAxis::RIGHT_TRIGGER);
    
    return state;
}

void JNIBridge::registerGamepadCallback(std::function<void(int, bool)> callback) {
    gamepadCallback_ = callback;
}

namespace d2::android {

GameEngineWrapper::GameEngineWrapper() 
    : initialized_(false), surface_width_(0), surface_height_(0) {
    engine_ = std::make_unique<d2::GameEngine>();
}

GameEngineWrapper::~GameEngineWrapper() {
    if (engine_ && engine_->isRunning()) {
        engine_->stop();
    }
}

bool GameEngineWrapper::initialize() {
    if (initialized_) {
        return true;
    }
    
    if (!engine_) {
        return false;
    }
    
    // Initialize the game engine
    if (!engine_->initialize()) {
        return false;
    }
    
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
    
    // Re-initialize with asset path
    return engine_->initialize(asset_path);
}

void GameEngineWrapper::onTouchEvent(float x, float y, int action) {
    if (!initialized_ || !engine_) {
        return;
    }
    
    // Convert Android action to TouchAction enum
    d2::input::TouchAction touchAction;
    switch (action) {
        case 0: // ACTION_DOWN
            touchAction = d2::input::TouchAction::DOWN;
            break;
        case 1: // ACTION_UP
            touchAction = d2::input::TouchAction::UP;
            break;
        case 2: // ACTION_MOVE
            touchAction = d2::input::TouchAction::MOVE;
            break;
        case 3: // ACTION_CANCEL
            touchAction = d2::input::TouchAction::CANCEL;
            break;
        default:
            return; // Unknown action
    }
    
    // Process touch input through the game engine
    engine_->processTouchInput(x, y, touchAction);
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
    
    // Update screen size for touch input
    if (engine_) {
        engine_->setScreenSize(width, height);
    }
    
    // Start the engine if not already running
    if (!engine_->isRunning()) {
        engine_->start();
    }
}

void GameEngineWrapper::renderFrame() {
    if (!initialized_) {
        return;
    }
    
    if (surface_width_ <= 0 || surface_height_ <= 0) {
        return;
    }
    
    if (engine_ && engine_->isRunning()) {
        engine_->renderFrame();
    }
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