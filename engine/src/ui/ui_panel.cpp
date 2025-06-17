#include "ui/ui_panel.h"

namespace d2 {

bool UIPanel::handleTouchInput(float x, float y, TouchEventType type) {
    // First check if the panel itself would handle it
    bool in_panel_bounds = UIElement::handleTouchInput(x, y, type);
    
    if (!in_panel_bounds) {
        return false;
    }
    
    // If we're in the panel bounds, try to propagate to children
    // Iterate in reverse order so top-most children get first chance
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        auto& child = *it;
        
        // Touch coordinates are already in absolute coordinates
        // Children's positions are relative to panel, so we need to check
        // if the absolute touch position is within the child's absolute bounds
        if (child && child->handleTouchInput(x, y, type)) {
            return true;  // Child handled it
        }
    }
    
    // No child handled it, but we're still in panel bounds
    return true;
}

} // namespace d2