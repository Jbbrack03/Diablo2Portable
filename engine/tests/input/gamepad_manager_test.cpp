#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "input/gamepad_manager.h"
#include "input/android_input.h"

using namespace d2::input;
using ::testing::Return;

class MockAndroidInput : public AndroidInput {
public:
    MOCK_METHOD(std::vector<int>, getConnectedDevices, (), (const, override));
    MOCK_METHOD(bool, isGamepad, (int deviceId), (const, override));
};

class GamepadManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockInput = std::make_unique<MockAndroidInput>();
    }

    std::unique_ptr<MockAndroidInput> mockInput;
};

TEST_F(GamepadManagerTest, DetectController) {
    GamepadManager gamepad(mockInput.get());
    
    // Set up expectation: one gamepad device is connected
    EXPECT_CALL(*mockInput, getConnectedDevices())
        .WillOnce(Return(std::vector<int>{1001}));
    
    EXPECT_CALL(*mockInput, isGamepad(1001))
        .WillOnce(Return(true));
    
    EXPECT_TRUE(gamepad.hasController());
}

TEST_F(GamepadManagerTest, NoControllerConnected) {
    GamepadManager gamepad(mockInput.get());
    
    // Set up expectation: no devices connected
    EXPECT_CALL(*mockInput, getConnectedDevices())
        .WillOnce(Return(std::vector<int>{}));
    
    EXPECT_FALSE(gamepad.hasController());
}