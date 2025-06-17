#include "ui/ui_button.h"

namespace d2 {

bool UIButton::handleTouchInput(float x, float y, TouchEventType type) {
    // First check if the base class would handle it (bounds check, enabled, visible)
    bool in_bounds = UIElement::handleTouchInput(x, y, type);
    
    switch (type) {
        case TouchEventType::TOUCH_DOWN:
            if (in_bounds) {
                pressed_ = true;
                return true;
            }
            break;
            
        case TouchEventType::TOUCH_UP:
            if (pressed_) {
                // We were pressed, check if we should trigger click
                if (in_bounds && onClick_) {
                    onClick_();
                }
                pressed_ = false;
                return in_bounds; // Return true if touch up was within bounds
            }
            // Clear pressed state even if touch up is outside
            pressed_ = false;
            break;
            
        case TouchEventType::TOUCH_MOVE:
            // Update hover state based on whether we're in bounds
            hovered_ = in_bounds;
            return in_bounds;
            
        case TouchEventType::TOUCH_CANCEL:
            // Cancel any ongoing interaction
            pressed_ = false;
            hovered_ = false;
            break;
    }
    
    return in_bounds;
}

} // namespace d2