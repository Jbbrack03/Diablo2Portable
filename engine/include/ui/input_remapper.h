#pragma once

#include "android/gamepad_handler.h"
#include <unordered_map>

namespace d2 {

class InputRemapper {
public:
    InputRemapper() {
        // Initialize default mapping (each button maps to itself)
        for (int i = 0; i <= static_cast<int>(GamepadButton::RIGHT_STICK); ++i) {
            GamepadButton button = static_cast<GamepadButton>(i);
            buttonMap_[button] = button;
        }
    }

    GamepadButton getMappedButton(GamepadButton physicalButton) const {
        auto it = buttonMap_.find(physicalButton);
        if (it != buttonMap_.end()) {
            return it->second;
        }
        return physicalButton;
    }

private:
    std::unordered_map<GamepadButton, GamepadButton> buttonMap_;
};

} // namespace d2