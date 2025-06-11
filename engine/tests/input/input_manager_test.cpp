#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "input/input_manager.h"
#include "input/gamepad.h"

using namespace d2::input;
using ::testing::Return;

// Define button constants for testing
constexpr int BUTTON_A = 0;
constexpr int BUTTON_B = 1;
constexpr int BUTTON_X = 2;
constexpr int BUTTON_Y = 3;

// Define axis constants for testing
constexpr int AXIS_LEFT_X = 0;
constexpr int AXIS_LEFT_Y = 1;
constexpr int AXIS_RIGHT_X = 2;
constexpr int AXIS_RIGHT_Y = 3;

class MockGamepad : public Gamepad {
public:
    MOCK_METHOD(bool, getButton, (int buttonId), (const, override));
    MOCK_METHOD(float, getAxis, (int axisId), (const, override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
};

class InputManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockGamepad = std::make_unique<MockGamepad>();
    }

    std::unique_ptr<MockGamepad> mockGamepad;
};

TEST_F(InputManagerTest, ReadButtonStates) {
    InputManager input(mockGamepad.get());
    
    // Set up expectations
    EXPECT_CALL(*mockGamepad, isConnected())
        .WillRepeatedly(Return(true));
        
    EXPECT_CALL(*mockGamepad, getButton(BUTTON_A))
        .WillOnce(Return(true));
        
    EXPECT_CALL(*mockGamepad, getButton(BUTTON_B))
        .WillOnce(Return(false));
    
    input.update();
    
    EXPECT_TRUE(input.isButtonPressed(GameButton::ACTION));
    EXPECT_FALSE(input.isButtonPressed(GameButton::CANCEL));
}

TEST_F(InputManagerTest, ReadAxisValues) {
    InputManager input(mockGamepad.get());
    
    // Set up expectations
    EXPECT_CALL(*mockGamepad, isConnected())
        .WillRepeatedly(Return(true));
        
    EXPECT_CALL(*mockGamepad, getAxis(AXIS_LEFT_X))
        .WillOnce(Return(0.5f));
        
    EXPECT_CALL(*mockGamepad, getAxis(AXIS_LEFT_Y))
        .WillOnce(Return(0.7f));
    
    input.update();
    
    auto movement = input.getMovement();
    EXPECT_NEAR(movement.x, 0.5f, 0.01f);
    EXPECT_NEAR(movement.y, 0.7f, 0.01f);
}