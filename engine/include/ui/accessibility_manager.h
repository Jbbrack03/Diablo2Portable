#pragma once

namespace d2 {

class AccessibilityManager {
public:
    AccessibilityManager() : textScale_(1.0f) {}

    float getTextScale() const { return textScale_; }

private:
    float textScale_;
};

} // namespace d2