#include <gtest/gtest.h>
#include "input/android_input_impl.h"

using namespace d2::input;

// Test constants matching Android input source types
constexpr int AINPUT_SOURCE_GAMEPAD = 0x00000401;
constexpr int AINPUT_SOURCE_KEYBOARD = 0x00000101;
constexpr int AINPUT_SOURCE_MOUSE = 0x00002002;

class AndroidInputTest : public ::testing::Test {
protected:
    void SetUp() override {
        // We'll test the real AndroidInput implementation
    }
};

TEST_F(AndroidInputTest, CreateAndroidInput) {
    AndroidInputImpl androidInput;
    
    // Should start with no connected devices
    auto devices = androidInput.getConnectedDevices();
    EXPECT_TRUE(devices.empty());
}

// Test adding a gamepad device
TEST_F(AndroidInputTest, AddGamepadDevice) {
    AndroidInputImpl androidInput;
    
    // Add a gamepad device
    androidInput.addDevice(1001, AINPUT_SOURCE_GAMEPAD);
    
    auto devices = androidInput.getConnectedDevices();
    EXPECT_EQ(devices.size(), 1);
    EXPECT_EQ(devices[0], 1001);
    EXPECT_TRUE(androidInput.isGamepad(1001));
}

// Test adding non-gamepad devices
TEST_F(AndroidInputTest, AddNonGamepadDevices) {
    AndroidInputImpl androidInput;
    
    // Add keyboard and mouse
    androidInput.addDevice(2001, AINPUT_SOURCE_KEYBOARD);
    androidInput.addDevice(2002, AINPUT_SOURCE_MOUSE);
    
    auto devices = androidInput.getConnectedDevices();
    EXPECT_EQ(devices.size(), 2);
    
    // Neither should be identified as gamepad
    EXPECT_FALSE(androidInput.isGamepad(2001));
    EXPECT_FALSE(androidInput.isGamepad(2002));
}

// Test removing a device
TEST_F(AndroidInputTest, RemoveDevice) {
    AndroidInputImpl androidInput;
    
    // Add some devices
    androidInput.addDevice(1001, AINPUT_SOURCE_GAMEPAD);
    androidInput.addDevice(2001, AINPUT_SOURCE_KEYBOARD);
    
    EXPECT_EQ(androidInput.getConnectedDevices().size(), 2);
    
    // Remove one device
    androidInput.removeDevice(1001);
    
    auto devices = androidInput.getConnectedDevices();
    EXPECT_EQ(devices.size(), 1);
    EXPECT_EQ(devices[0], 2001);
    
    // Removed device should return false for isGamepad
    EXPECT_FALSE(androidInput.isGamepad(1001));
}