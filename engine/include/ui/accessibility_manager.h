#pragma once

namespace d2 {

class AccessibilityManager {
public:
    AccessibilityManager() : textScale_(1.0f) {}

    float getTextScale() const { return textScale_; }
    void setTextScale(float scale) { textScale_ = scale; }

private:
    float textScale_;
};

} // namespace d2