#include "android/gamepad_handler.h"

// Android KeyEvent keycodes
constexpr int KEYCODE_BUTTON_A = 96;
constexpr int KEYCODE_BUTTON_B = 97;
constexpr int KEYCODE_BUTTON_X = 99;
constexpr int KEYCODE_BUTTON_Y = 100;
constexpr int KEYCODE_BUTTON_L1 = 102;
constexpr int KEYCODE_BUTTON_R1 = 103;
constexpr int KEYCODE_BUTTON_L2 = 104;
constexpr int KEYCODE_BUTTON_R2 = 105;
constexpr int KEYCODE_BUTTON_START = 108;
constexpr int KEYCODE_BUTTON_SELECT = 109;
constexpr int KEYCODE_DPAD_UP = 19;
constexpr int KEYCODE_DPAD_DOWN = 20;
constexpr int KEYCODE_DPAD_LEFT = 21;
constexpr int KEYCODE_DPAD_RIGHT = 22;

// Android MotionEvent axis codes
constexpr int AXIS_X = 0;
constexpr int AXIS_Y = 1;
constexpr int AXIS_Z = 11;  // Right stick X
constexpr int AXIS_RZ = 14; // Right stick Y
constexpr int AXIS_HAT_X = 15;
constexpr int AXIS_HAT_Y = 16;
constexpr int AXIS_LTRIGGER = 17;
constexpr int AXIS_RTRIGGER = 18;

// Android input source
constexpr int SOURCE_JOYSTICK = 0x1000010;

// Action codes
constexpr int ACTION_DOWN = 0;
constexpr int ACTION_UP = 1;

bool GamepadHandler::handleKeyEvent(int keyCode, int action) {
    GamepadButton button;
    bool isGamepadButton = true;
    
    switch (keyCode) {
        case KEYCODE_BUTTON_A:
            button = GamepadButton::A;
            break;
        case KEYCODE_BUTTON_B:
            button = GamepadButton::B;
            break;
        case KEYCODE_BUTTON_X:
            button = GamepadButton::X;
            break;
        case KEYCODE_BUTTON_Y:
            button = GamepadButton::Y;
            break;
        case KEYCODE_BUTTON_L1:
            button = GamepadButton::LEFT_BUMPER;
            break;
        case KEYCODE_BUTTON_R1:
            button = GamepadButton::RIGHT_BUMPER;
            break;
        case KEYCODE_BUTTON_L2:
            button = GamepadButton::LEFT_TRIGGER;
            break;
        case KEYCODE_BUTTON_R2:
            button = GamepadButton::RIGHT_TRIGGER;
            break;
        case KEYCODE_BUTTON_START:
            button = GamepadButton::START;
            break;
        case KEYCODE_BUTTON_SELECT:
            button = GamepadButton::SELECT;
            break;
        case KEYCODE_DPAD_UP:
            button = GamepadButton::DPAD_UP;
            break;
        case KEYCODE_DPAD_DOWN:
            button = GamepadButton::DPAD_DOWN;
            break;
        case KEYCODE_DPAD_LEFT:
            button = GamepadButton::DPAD_LEFT;
            break;
        case KEYCODE_DPAD_RIGHT:
            button = GamepadButton::DPAD_RIGHT;
            break;
        default:
            isGamepadButton = false;
            break;
    }
    
    if (isGamepadButton) {
        m_buttonStates[button] = (action == ACTION_DOWN);
        return true;
    }
    
    return false;
}

bool GamepadHandler::handleMotionEvent(int axisCode1, float value1, int axisCode2, float value2, int source) {
    // Only handle joystick events
    if ((source & SOURCE_JOYSTICK) == 0) {
        return false;
    }
    
    bool handled = false;
    
    // Handle first axis
    if (axisCode1 == AXIS_X) {
        m_axisValues[GamepadAxis::LEFT_X] = applyDeadzone(value1);
        handled = true;
    } else if (axisCode1 == AXIS_Y) {
        m_axisValues[GamepadAxis::LEFT_Y] = applyDeadzone(value1);
        handled = true;
    } else if (axisCode1 == AXIS_Z) {
        m_axisValues[GamepadAxis::RIGHT_X] = applyDeadzone(value1);
        handled = true;
    } else if (axisCode1 == AXIS_RZ) {
        m_axisValues[GamepadAxis::RIGHT_Y] = applyDeadzone(value1);
        handled = true;
    } else if (axisCode1 == AXIS_HAT_X) {
        // D-pad horizontal
        m_buttonStates[GamepadButton::DPAD_LEFT] = (value1 < -0.5f);
        m_buttonStates[GamepadButton::DPAD_RIGHT] = (value1 > 0.5f);
        handled = true;
    } else if (axisCode1 == AXIS_HAT_Y) {
        // D-pad vertical
        m_buttonStates[GamepadButton::DPAD_UP] = (value1 < -0.5f);
        m_buttonStates[GamepadButton::DPAD_DOWN] = (value1 > 0.5f);
        handled = true;
    }
    
    // Handle second axis
    if (axisCode2 == AXIS_X) {
        m_axisValues[GamepadAxis::LEFT_X] = applyDeadzone(value2);
        handled = true;
    } else if (axisCode2 == AXIS_Y) {
        m_axisValues[GamepadAxis::LEFT_Y] = applyDeadzone(value2);
        handled = true;
    } else if (axisCode2 == AXIS_Z) {
        m_axisValues[GamepadAxis::RIGHT_X] = applyDeadzone(value2);
        handled = true;
    } else if (axisCode2 == AXIS_RZ) {
        m_axisValues[GamepadAxis::RIGHT_Y] = applyDeadzone(value2);
        handled = true;
    } else if (axisCode2 == AXIS_HAT_X) {
        // D-pad horizontal
        m_buttonStates[GamepadButton::DPAD_LEFT] = (value2 < -0.5f);
        m_buttonStates[GamepadButton::DPAD_RIGHT] = (value2 > 0.5f);
        handled = true;
    } else if (axisCode2 == AXIS_HAT_Y) {
        // D-pad vertical
        m_buttonStates[GamepadButton::DPAD_UP] = (value2 < -0.5f);
        m_buttonStates[GamepadButton::DPAD_DOWN] = (value2 > 0.5f);
        handled = true;
    }
    
    return handled;
}

bool GamepadHandler::isButtonPressed(GamepadButton button) const {
    auto it = m_buttonStates.find(button);
    return it != m_buttonStates.end() && it->second;
}

float GamepadHandler::getAxisValue(GamepadAxis axis) const {
    auto it = m_axisValues.find(axis);
    return it != m_axisValues.end() ? it->second : 0.0f;
}