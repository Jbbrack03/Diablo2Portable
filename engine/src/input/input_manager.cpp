#include "input/input_manager.h"
#include "input/gamepad.h"

namespace d2::input {

// Button mapping from physical buttons to game buttons
constexpr int BUTTON_A = 0;
constexpr int BUTTON_B = 1;

// Axis mapping
constexpr int AXIS_LEFT_X = 0;
constexpr int AXIS_LEFT_Y = 1;

InputManager::InputManager(Gamepad* gamepad)
    : gamepad_(gamepad), leftStick_(0.0f, 0.0f) {
}

void InputManager::update() {
    if (!gamepad_ || !gamepad_->isConnected()) {
        return;
    }
    
    // Map physical buttons to game buttons
    buttonStates_[GameButton::ACTION] = gamepad_->getButton(BUTTON_A);
    buttonStates_[GameButton::CANCEL] = gamepad_->getButton(BUTTON_B);
    
    // Read analog stick values
    leftStick_.x = gamepad_->getAxis(AXIS_LEFT_X);
    leftStick_.y = gamepad_->getAxis(AXIS_LEFT_Y);
}

bool InputManager::isButtonPressed(GameButton button) const {
    auto it = buttonStates_.find(button);
    return it != buttonStates_.end() ? it->second : false;
}

glm::vec2 InputManager::getMovement() const {
    return leftStick_;
}

} // namespace d2::input