#pragma once

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

private:
    float textScale_;
    ColorblindMode colorblindMode_;
};

} // namespace d2