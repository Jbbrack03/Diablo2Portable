#include "input/input_manager.h"
#include "input/gamepad.h"

namespace d2::input {

// Button mapping from physical buttons to game buttons
constexpr int BUTTON_A = 0;
constexpr int BUTTON_B = 1;

InputManager::InputManager(Gamepad* gamepad)
    : gamepad_(gamepad) {
}

void InputManager::update() {
    if (!gamepad_ || !gamepad_->isConnected()) {
        return;
    }
    
    // Map physical buttons to game buttons
    buttonStates_[GameButton::ACTION] = gamepad_->getButton(BUTTON_A);
    buttonStates_[GameButton::CANCEL] = gamepad_->getButton(BUTTON_B);
}

bool InputManager::isButtonPressed(GameButton button) const {
    auto it = buttonStates_.find(button);
    return it != buttonStates_.end() ? it->second : false;
}

} // namespace d2::input