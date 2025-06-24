#include <gtest/gtest.h>
#include "input/android_gamepad.h"

using namespace d2::input;

// Test constants for button and axis IDs
constexpr int BUTTON_A = 0;
constexpr int BUTTON_B = 1;
constexpr int AXIS_LEFT_X = 0;
constexpr int AXIS_LEFT_Y = 1;

class AndroidGamepadTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
};

// Test creating an Android gamepad
TEST_F(AndroidGamepadTest, CreateAndroidGamepad) {
    AndroidGamepad gamepad(1001); // Device ID
    
    // Should start connected
    EXPECT_TRUE(gamepad.isConnected());
    
    // Should have no buttons pressed initially
    EXPECT_FALSE(gamepad.getButton(BUTTON_A));
    EXPECT_FALSE(gamepad.getButton(BUTTON_B));
    
    // Axes should be centered (0.0)
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_X), 0.0f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_Y), 0.0f);
}

// Test button state updates
TEST_F(AndroidGamepadTest, UpdateButtonStates) {
    AndroidGamepad gamepad(1001);
    
    // Press button A
    gamepad.setButton(BUTTON_A, true);
    EXPECT_TRUE(gamepad.getButton(BUTTON_A));
    EXPECT_FALSE(gamepad.getButton(BUTTON_B));
    
    // Press button B
    gamepad.setButton(BUTTON_B, true);
    EXPECT_TRUE(gamepad.getButton(BUTTON_A));
    EXPECT_TRUE(gamepad.getButton(BUTTON_B));
    
    // Release button A
    gamepad.setButton(BUTTON_A, false);
    EXPECT_FALSE(gamepad.getButton(BUTTON_A));
    EXPECT_TRUE(gamepad.getButton(BUTTON_B));
}

// Test axis value updates
TEST_F(AndroidGamepadTest, UpdateAxisValues) {
    AndroidGamepad gamepad(1001);
    
    // Move left stick right
    gamepad.setAxis(AXIS_LEFT_X, 1.0f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_X), 1.0f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_Y), 0.0f);
    
    // Move left stick up-right
    gamepad.setAxis(AXIS_LEFT_Y, -1.0f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_X), 1.0f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_Y), -1.0f);
    
    // Center the stick
    gamepad.setAxis(AXIS_LEFT_X, 0.0f);
    gamepad.setAxis(AXIS_LEFT_Y, 0.0f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_X), 0.0f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_Y), 0.0f);
}

// Test disconnection
TEST_F(AndroidGamepadTest, DisconnectGamepad) {
    AndroidGamepad gamepad(1001);
    
    EXPECT_TRUE(gamepad.isConnected());
    
    // Disconnect the gamepad
    gamepad.setConnected(false);
    EXPECT_FALSE(gamepad.isConnected());
    
    // Reconnect
    gamepad.setConnected(true);
    EXPECT_TRUE(gamepad.isConnected());
}

// Test deadzone application
TEST_F(AndroidGamepadTest, ApplyDeadzone) {
    AndroidGamepad gamepad(1001);
    gamepad.setDeadzone(0.15f); // 15% deadzone
    
    // Values within deadzone should be zero
    gamepad.setAxis(AXIS_LEFT_X, 0.1f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_X), 0.0f);
    
    gamepad.setAxis(AXIS_LEFT_X, -0.1f);
    EXPECT_FLOAT_EQ(gamepad.getAxis(AXIS_LEFT_X), 0.0f);
    
    // Values outside deadzone should be scaled
    gamepad.setAxis(AXIS_LEFT_X, 0.5f);
    // With 15% deadzone, 0.5 should map to approximately 0.412
    float expected = (0.5f - 0.15f) / (1.0f - 0.15f);
    EXPECT_NEAR(gamepad.getAxis(AXIS_LEFT_X), expected, 0.001f);
}