#pragma once

#include <string>
#include <fstream>
#include <sstream>

namespace d2 {

enum class ColorblindMode {
    None,
    Protanopia,
    Deuteranopia,
    Tritanopia
};

class AccessibilityManager {
public:
    AccessibilityManager() : textScale_(1.0f), colorblindMode_(ColorblindMode::None) {}

    float getTextScale() const { return textScale_; }
    void setTextScale(float scale) { textScale_ = scale; }
    
    ColorblindMode getColorblindMode() const { return colorblindMode_; }
    void setColorblindMode(ColorblindMode mode) { colorblindMode_ = mode; }
    
    bool saveSettings(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) return false;
        
        file << "text_scale=" << textScale_ << "\n";
        file << "colorblind_mode=" << static_cast<int>(colorblindMode_) << "\n";
        
        file.close();
        return true;
    }
    
    bool loadSettings(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, '=')) {
                std::string value;
                if (std::getline(iss, value)) {
                    if (key == "text_scale") {
                        textScale_ = std::stof(value);
                    } else if (key == "colorblind_mode") {
                        colorblindMode_ = static_cast<ColorblindMode>(std::stoi(value));
                    }
                }
            }
        }
        
        file.close();
        return true;
    }

private:
    float textScale_;
    ColorblindMode colorblindMode_;
};

} // namespace d2