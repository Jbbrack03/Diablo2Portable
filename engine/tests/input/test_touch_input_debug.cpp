#include <gtest/gtest.h>
#include "input/touch_input.h"
#include <glm/glm.hpp>
#include <iostream>

namespace d2::input {

TEST(TouchInputDebug, VirtualJoystickCalculation) {
    TouchInput touchInput;
    touchInput.setScreenSize(800, 600);
    touchInput.enableVirtualJoystick(true);
    
    // Test joystick area detection
    touchInput.onTouchEvent(150.0f, 550.0f, TouchAction::DOWN);
    
    // Move within joystick
    touchInput.onTouchEvent(200.0f, 550.0f, TouchAction::MOVE);
    
    auto gameInput = touchInput.getGameInput();
    
    std::cout << "Game input - isMoving: " << gameInput.isMoving << std::endl;
    std::cout << "Game input - moveDirection: (" 
              << gameInput.moveDirection.x << ", " 
              << gameInput.moveDirection.y << ")" << std::endl;
    
    // Also test pure direct movement
    TouchInput touchInput2;
    touchInput2.setScreenSize(800, 600);
    touchInput2.enableVirtualJoystick(false);
    
    touchInput2.onTouchEvent(200.0f, 550.0f, TouchAction::DOWN);
    auto gameInput2 = touchInput2.getGameInput();
    
    std::cout << "Direct movement - moveDirection: (" 
              << gameInput2.moveDirection.x << ", " 
              << gameInput2.moveDirection.y << ")" << std::endl;
}

} // namespace d2::input