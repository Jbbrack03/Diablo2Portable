#pragma once

#include <unordered_map>
#include <cmath>

enum class GamepadButton {
    A,
    B,
    X,
    Y,
    LEFT_BUMPER,
    RIGHT_BUMPER,
    LEFT_TRIGGER,
    RIGHT_TRIGGER,
    START,
    SELECT,
    DPAD_UP,
    DPAD_DOWN,
    DPAD_LEFT,
    DPAD_RIGHT,
    LEFT_STICK,
    RIGHT_STICK
};

enum class GamepadAxis {
    LEFT_X,
    LEFT_Y,
    RIGHT_X,
    RIGHT_Y,
    LEFT_TRIGGER,
    RIGHT_TRIGGER
};

class GamepadHandler {
public:
    GamepadHandler() : m_deadzone(DEFAULT_DEADZONE) {}
    ~GamepadHandler() = default;
    
    // Handle Android KeyEvent for gamepad buttons
    bool handleKeyEvent(int keyCode, int action);
    
    // Handle Android MotionEvent for analog sticks
    bool handleMotionEvent(int axisCode1, float value1, int axisCode2, float value2, int source);
    
    // Query button states
    bool isButtonPressed(GamepadButton button) const;
    
    // Query axis values
    float getAxisValue(GamepadAxis axis) const;
    
    // Configure deadzone for analog sticks
    void setDeadzone(float deadzone) { m_deadzone = deadzone; }
    
private:
    static constexpr float DEFAULT_DEADZONE = 0.1f;
    
    // Button state tracking
    std::unordered_map<GamepadButton, bool> m_buttonStates;
    
    // Axis value tracking
    std::unordered_map<GamepadAxis, float> m_axisValues;
    
    // Deadzone threshold
    float m_deadzone;
    
    // Helper to apply deadzone
    float applyDeadzone(float value) const {
        return std::abs(value) < m_deadzone ? 0.0f : value;
    }
};