#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "android/jni_bridge.h"

using namespace testing;

class GamepadJNITest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize JNI bridge would normally happen here
        // For testing, we'll simulate JNI calls
    }
};

TEST_F(GamepadJNITest, HandleKeyEventFromJava) {
    // Test that JNI bridge correctly forwards key events to gamepad handler
    int keyCode = 96; // KEYCODE_BUTTON_A
    int action = 0; // ACTION_DOWN
    
    bool handled = JNIBridge::handleKeyEvent(keyCode, action);
    
    EXPECT_TRUE(handled);
}

TEST_F(GamepadJNITest, HandleMotionEventFromJava) {
    // Test that JNI bridge correctly forwards motion events to gamepad handler
    float axisX = 0.7f;
    float axisY = -0.5f;
    int source = 0x1000010; // SOURCE_JOYSTICK
    
    bool handled = JNIBridge::handleMotionEvent(0, axisX, 1, axisY, source);
    
    EXPECT_TRUE(handled);
}

TEST_F(GamepadJNITest, GetControllerStateForUI) {
    // Test that we can query controller state for UI updates
    JNIBridge::handleKeyEvent(96, 0); // Press A button
    
    ControllerState state = JNIBridge::getControllerState();
    
    EXPECT_TRUE(state.buttons[0]); // A button pressed
    EXPECT_FALSE(state.buttons[1]); // B button not pressed
}

TEST_F(GamepadJNITest, RegisterGamepadEventCallback) {
    // Test that we can register callbacks for gamepad events
    bool callbackTriggered = false;
    auto callback = [&callbackTriggered](int button, bool pressed) {
        callbackTriggered = true;
    };
    
    JNIBridge::registerGamepadCallback(callback);
    JNIBridge::handleKeyEvent(96, 0); // Press A button
    
    EXPECT_TRUE(callbackTriggered);
}