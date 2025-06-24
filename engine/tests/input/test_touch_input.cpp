#include <gtest/gtest.h>
#include "input/touch_input.h"
#include <glm/glm.hpp>

namespace d2::input {

class TouchInputTest : public ::testing::Test {
protected:
    void SetUp() override {
        touchInput = std::make_unique<TouchInput>();
    }
    
    std::unique_ptr<TouchInput> touchInput;
};

TEST_F(TouchInputTest, ProcessSingleTouch) {
    // Simulate touch down at position (100, 200)
    touchInput->onTouchEvent(100.0f, 200.0f, TouchAction::DOWN);
    
    EXPECT_TRUE(touchInput->isTouching());
    EXPECT_EQ(touchInput->getTouchPosition(), glm::vec2(100.0f, 200.0f));
}

TEST_F(TouchInputTest, ProcessTouchMove) {
    // Touch down
    touchInput->onTouchEvent(100.0f, 200.0f, TouchAction::DOWN);
    
    // Move to new position
    touchInput->onTouchEvent(150.0f, 250.0f, TouchAction::MOVE);
    
    EXPECT_TRUE(touchInput->isTouching());
    EXPECT_EQ(touchInput->getTouchPosition(), glm::vec2(150.0f, 250.0f));
    EXPECT_EQ(touchInput->getTouchDelta(), glm::vec2(50.0f, 50.0f));
}

TEST_F(TouchInputTest, ProcessTouchUp) {
    // Touch down
    touchInput->onTouchEvent(100.0f, 200.0f, TouchAction::DOWN);
    EXPECT_TRUE(touchInput->isTouching());
    
    // Touch up
    touchInput->onTouchEvent(100.0f, 200.0f, TouchAction::UP);
    EXPECT_FALSE(touchInput->isTouching());
}

TEST_F(TouchInputTest, ConvertToGameInput) {
    // Touch in the left side of screen should trigger movement
    touchInput->setScreenSize(800, 600);
    
    // Touch on left side (movement area)
    touchInput->onTouchEvent(200.0f, 300.0f, TouchAction::DOWN);
    
    auto gameInput = touchInput->getGameInput();
    EXPECT_TRUE(gameInput.isMoving);
    EXPECT_NE(gameInput.moveDirection, glm::vec2(0.0f, 0.0f));
}

TEST_F(TouchInputTest, DetectTapGesture) {
    // Quick tap should be detected as action
    touchInput->onTouchEvent(400.0f, 300.0f, TouchAction::DOWN);
    touchInput->onTouchEvent(400.0f, 300.0f, TouchAction::UP);
    
    EXPECT_TRUE(touchInput->wasTapped());
    EXPECT_EQ(touchInput->getLastTapPosition(), glm::vec2(400.0f, 300.0f));
}

TEST_F(TouchInputTest, VirtualJoystickMovement) {
    touchInput->setScreenSize(800, 600);
    touchInput->enableVirtualJoystick(true);
    
    // Joystick center is at (150, 450) after setScreenSize
    // height - radius - 50 = 600 - 100 - 50 = 450
    // Touch to the right of center for pure horizontal movement
    touchInput->onTouchEvent(200.0f, 450.0f, TouchAction::DOWN);
    
    auto gameInput = touchInput->getGameInput();
    EXPECT_TRUE(gameInput.isMoving);
    // Direction from center (150, 450) to touch (200, 450) is pure right
    EXPECT_GT(gameInput.moveDirection.x, 0.0f); // Moving right
    EXPECT_NEAR(gameInput.moveDirection.y, 0.0f, 0.01f); // No vertical movement
}

} // namespace d2::input