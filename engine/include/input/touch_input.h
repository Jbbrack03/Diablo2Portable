#pragma once
#include <glm/glm.hpp>
#include <chrono>

namespace d2::input {

enum class TouchAction {
    DOWN = 0,
    UP = 1,
    MOVE = 2,
    CANCEL = 3
};

struct GameInput {
    bool isMoving = false;
    glm::vec2 moveDirection{0.0f, 0.0f};
    bool actionPressed = false;
    bool skill1Pressed = false;
    bool skill2Pressed = false;
};

class TouchInput {
public:
    TouchInput() = default;
    ~TouchInput() = default;
    
    // Process raw touch events
    void onTouchEvent(float x, float y, TouchAction action);
    
    // Configure screen dimensions for input mapping
    void setScreenSize(int width, int height);
    
    // Enable/disable virtual joystick
    void enableVirtualJoystick(bool enabled);
    
    // Query touch state
    bool isTouching() const { return touching_; }
    glm::vec2 getTouchPosition() const { return currentPosition_; }
    glm::vec2 getTouchDelta() const { return currentPosition_ - lastPosition_; }
    
    // Gesture detection
    bool wasTapped() const { return tapped_; }
    glm::vec2 getLastTapPosition() const { return tapPosition_; }
    
    // Get converted game input
    GameInput getGameInput() const;
    
    // Clear tap state (call after processing)
    void clearTapState() { tapped_ = false; }
    
private:
    bool touching_ = false;
    glm::vec2 currentPosition_{0.0f, 0.0f};
    glm::vec2 lastPosition_{0.0f, 0.0f};
    glm::vec2 startPosition_{0.0f, 0.0f};
    
    // Screen dimensions
    int screenWidth_ = 800;
    int screenHeight_ = 600;
    
    // Virtual joystick settings
    bool virtualJoystickEnabled_ = false;
    float joystickRadius_ = 100.0f;
    glm::vec2 joystickCenter_{150.0f, 450.0f}; // Bottom left
    
    // Tap detection
    bool tapped_ = false;
    glm::vec2 tapPosition_{0.0f, 0.0f};
    std::chrono::steady_clock::time_point touchDownTime_;
    static constexpr auto TAP_DURATION = std::chrono::milliseconds(300);
    static constexpr float TAP_DISTANCE_THRESHOLD = 10.0f;
    
    // Helper methods
    bool isInJoystickArea(const glm::vec2& pos) const;
    glm::vec2 calculateJoystickDirection(const glm::vec2& pos) const;
    bool isInActionArea(const glm::vec2& pos) const;
};

} // namespace d2::input