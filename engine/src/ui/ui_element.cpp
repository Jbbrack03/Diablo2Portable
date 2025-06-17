#include "ui/ui_element.h"

namespace d2 {

bool UIElement::handleTouchInput(float x, float y, TouchEventType type) {
    // Check if the element is visible and enabled
    if (!isVisible() || !isEnabled()) {
        return false;
    }
    
    // Check if the touch is within the element's bounds
    float left = position_.x;
    float top = position_.y;
    float right = position_.x + size_.x;
    float bottom = position_.y + size_.y;
    
    if (x >= left && x < right && y >= top && y < bottom) {
        // Touch is within bounds
        return true;
    }
    
    return false;
}

} // namespace d2