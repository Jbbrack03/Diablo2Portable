#include "ui/ui_button.h"
#include "ui/ui_manager.h"

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

bool UIButton::handleControllerInput(ControllerAction action) {
    if (!isEnabled() || !isVisible()) {
        return false;
    }
    
    switch (action) {
        case ControllerAction::ACTION_BUTTON:
            if (isFocused()) {
                // Temporarily show pressed state
                pressed_ = true;
                
                // Trigger click callback
                if (onClick_) {
                    onClick_();
                }
                
                // Note: In a real implementation, we'd need a frame delay 
                // or timer to show the pressed state briefly
                pressed_ = false;
                
                return true;
            }
            break;
            
        case ControllerAction::NAVIGATE_UP:
        case ControllerAction::NAVIGATE_DOWN:
        case ControllerAction::NAVIGATE_LEFT:
        case ControllerAction::NAVIGATE_RIGHT:
        case ControllerAction::CANCEL_BUTTON:
            // Navigation and cancel should be handled by parent container
            return false;
    }
    
    return false;
}

} // namespace d2