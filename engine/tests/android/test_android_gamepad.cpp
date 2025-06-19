#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "android/gamepad_handler.h"

using namespace testing;

class AndroidGamepadTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = std::make_unique<GamepadHandler>();
    }

    std::unique_ptr<GamepadHandler> handler;
};

TEST_F(AndroidGamepadTest, HandleButtonPress) {
    // Test handling Android KeyEvent for gamepad button A
    int keyCode = 96; // KEYCODE_BUTTON_A
    int action = 0; // ACTION_DOWN
    
    bool handled = handler->handleKeyEvent(keyCode, action);
    
    EXPECT_TRUE(handled);
    EXPECT_TRUE(handler->isButtonPressed(GamepadButton::A));
}

TEST_F(AndroidGamepadTest, HandleButtonRelease) {
    // Test handling Android KeyEvent for gamepad button release
    int keyCode = 96; // KEYCODE_BUTTON_A
    
    handler->handleKeyEvent(keyCode, 0); // ACTION_DOWN
    EXPECT_TRUE(handler->isButtonPressed(GamepadButton::A));
    
    handler->handleKeyEvent(keyCode, 1); // ACTION_UP
    EXPECT_FALSE(handler->isButtonPressed(GamepadButton::A));
}

TEST_F(AndroidGamepadTest, HandleAnalogStick) {
    // Test handling Android MotionEvent for analog stick
    float axisX = 0.7f;
    float axisY = -0.5f;
    int source = 0x1000010; // SOURCE_JOYSTICK
    
    bool handled = handler->handleMotionEvent(0, axisX, 1, axisY, source);
    
    EXPECT_TRUE(handled);
    EXPECT_NEAR(handler->getAxisValue(GamepadAxis::LEFT_X), 0.7f, 0.01f);
    EXPECT_NEAR(handler->getAxisValue(GamepadAxis::LEFT_Y), -0.5f, 0.01f);
}

TEST_F(AndroidGamepadTest, HandleDPad) {
    // Test handling D-pad input
    float hatX = 1.0f;
    float hatY = 0.0f;
    int source = 0x1000010; // SOURCE_JOYSTICK
    
    bool handled = handler->handleMotionEvent(15, hatX, 16, hatY, source); // HAT_X, HAT_Y
    
    EXPECT_TRUE(handled);
    EXPECT_TRUE(handler->isButtonPressed(GamepadButton::DPAD_RIGHT));
    EXPECT_FALSE(handler->isButtonPressed(GamepadButton::DPAD_LEFT));
}

TEST_F(AndroidGamepadTest, IgnoreNonGamepadInput) {
    // Test that non-gamepad input is ignored
    int keyCode = 29; // KEYCODE_A (keyboard)
    int action = 0; // ACTION_DOWN
    
    bool handled = handler->handleKeyEvent(keyCode, action);
    
    EXPECT_FALSE(handled);
}

TEST_F(AndroidGamepadTest, MapAndroidButtonCodes) {
    // Test mapping of Android keycodes to gamepad buttons
    struct ButtonMapping {
        int androidKeyCode;
        GamepadButton expectedButton;
    };
    
    ButtonMapping mappings[] = {
        {96, GamepadButton::A},              // KEYCODE_BUTTON_A
        {97, GamepadButton::B},              // KEYCODE_BUTTON_B
        {99, GamepadButton::X},              // KEYCODE_BUTTON_X
        {100, GamepadButton::Y},             // KEYCODE_BUTTON_Y
        {102, GamepadButton::LEFT_BUMPER},   // KEYCODE_BUTTON_L1
        {103, GamepadButton::RIGHT_BUMPER},  // KEYCODE_BUTTON_R1
        {104, GamepadButton::LEFT_TRIGGER},  // KEYCODE_BUTTON_L2
        {105, GamepadButton::RIGHT_TRIGGER}, // KEYCODE_BUTTON_R2
        {108, GamepadButton::START},         // KEYCODE_BUTTON_START
        {109, GamepadButton::SELECT},        // KEYCODE_BUTTON_SELECT
    };
    
    for (const auto& mapping : mappings) {
        handler->handleKeyEvent(mapping.androidKeyCode, 0); // ACTION_DOWN
        EXPECT_TRUE(handler->isButtonPressed(mapping.expectedButton))
            << "Failed for keycode " << mapping.androidKeyCode;
        handler->handleKeyEvent(mapping.androidKeyCode, 1); // ACTION_UP
    }
}

TEST_F(AndroidGamepadTest, DeadzoneHandling) {
    // Test analog stick deadzone handling
    float smallValue = 0.05f; // Below typical deadzone
    float largeValue = 0.5f;  // Above deadzone
    int source = 0x1000010; // SOURCE_JOYSTICK
    
    handler->handleMotionEvent(0, smallValue, 1, smallValue, source);
    EXPECT_NEAR(handler->getAxisValue(GamepadAxis::LEFT_X), 0.0f, 0.01f);
    EXPECT_NEAR(handler->getAxisValue(GamepadAxis::LEFT_Y), 0.0f, 0.01f);
    
    handler->handleMotionEvent(0, largeValue, 1, largeValue, source);
    EXPECT_GT(handler->getAxisValue(GamepadAxis::LEFT_X), 0.1f);
    EXPECT_GT(handler->getAxisValue(GamepadAxis::LEFT_Y), 0.1f);
}