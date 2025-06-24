#include "input/touch_input.h"
#include <algorithm>
#include <cmath>

namespace d2::input {

void TouchInput::onTouchEvent(float x, float y, TouchAction action) {
    lastPosition_ = currentPosition_;
    currentPosition_ = glm::vec2(x, y);
    
    switch (action) {
        case TouchAction::DOWN:
            touching_ = true;
            startPosition_ = currentPosition_;
            touchDownTime_ = std::chrono::steady_clock::now();
            break;
            
        case TouchAction::UP:
            if (touching_) {
                // Check for tap gesture
                auto duration = std::chrono::steady_clock::now() - touchDownTime_;
                float distance = glm::length(currentPosition_ - startPosition_);
                
                if (duration <= TAP_DURATION && distance <= TAP_DISTANCE_THRESHOLD) {
                    tapped_ = true;
                    tapPosition_ = currentPosition_;
                }
            }
            touching_ = false;
            break;
            
        case TouchAction::MOVE:
            // Position already updated above
            break;
            
        case TouchAction::CANCEL:
            touching_ = false;
            break;
    }
}

void TouchInput::setScreenSize(int width, int height) {
    screenWidth_ = width;
    screenHeight_ = height;
    
    // Update virtual joystick position based on screen size
    joystickCenter_ = glm::vec2(
        joystickRadius_ + 50.0f,
        height - joystickRadius_ - 50.0f
    );
}

void TouchInput::enableVirtualJoystick(bool enabled) {
    virtualJoystickEnabled_ = enabled;
}

GameInput TouchInput::getGameInput() const {
    GameInput input;
    
    if (!touching_) {
        return input;
    }
    
    if (virtualJoystickEnabled_ && isInJoystickArea(currentPosition_)) {
        // Virtual joystick movement
        input.isMoving = true;
        input.moveDirection = calculateJoystickDirection(currentPosition_);
    } else if (currentPosition_.x < screenWidth_ / 2.0f) {
        // Direct touch movement (left half of screen for movement)
        input.isMoving = true;
        // Use a fixed movement center in the left half of the screen
        glm::vec2 movementCenter(screenWidth_ * 0.25f, screenHeight_ * 0.5f);
        glm::vec2 direction = currentPosition_ - movementCenter;
        
        // Ensure we have some movement even at the center
        if (glm::length(direction) > 5.0f) { // Dead zone of 5 pixels
            input.moveDirection = glm::normalize(direction);
        } else {
            // Default to slight right movement if too close to center
            input.moveDirection = glm::vec2(0.1f, 0.0f);
        }
    } else {
        // Right half of screen for actions
        input.actionPressed = isInActionArea(currentPosition_);
    }
    
    return input;
}

bool TouchInput::isInJoystickArea(const glm::vec2& pos) const {
    float distance = glm::length(pos - joystickCenter_);
    return distance <= joystickRadius_ * 1.5f; // Allow some margin
}

glm::vec2 TouchInput::calculateJoystickDirection(const glm::vec2& pos) const {
    glm::vec2 direction = pos - joystickCenter_;
    float distance = glm::length(direction);
    
    if (distance > 0.0f) {
        // Normalize and clamp to joystick radius
        direction = glm::normalize(direction);
        float strength = std::min(distance / joystickRadius_, 1.0f);
        return direction * strength;
    }
    
    return glm::vec2(0.0f, 0.0f);
}

bool TouchInput::isInActionArea(const glm::vec2& pos) const {
    // Action button area (right side of screen)
    return pos.x > screenWidth_ * 0.7f && pos.y > screenHeight_ * 0.5f;
}

} // namespace d2::input