#pragma once

#include "android/gamepad_handler.h"
#include <unordered_map>
#include <fstream>
#include <string>
#include <sstream>

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
    
    void setButtonMapping(GamepadButton physicalButton, GamepadButton logicalButton) {
        buttonMap_[physicalButton] = logicalButton;
    }
    
    bool saveMappings(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) return false;
        
        for (const auto& [physical, logical] : buttonMap_) {
            // Only save non-default mappings
            if (physical != logical) {
                file << static_cast<int>(physical) << "=" << static_cast<int>(logical) << "\n";
            }
        }
        
        file.close();
        return true;
    }
    
    bool loadMappings(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        // Reset to defaults first
        for (int i = 0; i <= static_cast<int>(GamepadButton::RIGHT_STICK); ++i) {
            GamepadButton button = static_cast<GamepadButton>(i);
            buttonMap_[button] = button;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                int physicalInt = std::stoi(key);
                int logicalInt = std::stoi(value);
                GamepadButton physical = static_cast<GamepadButton>(physicalInt);
                GamepadButton logical = static_cast<GamepadButton>(logicalInt);
                buttonMap_[physical] = logical;
            }
        }
        
        file.close();
        return true;
    }

private:
    std::unordered_map<GamepadButton, GamepadButton> buttonMap_;
};

} // namespace d2