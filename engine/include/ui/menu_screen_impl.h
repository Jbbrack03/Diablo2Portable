#pragma once

#include "menu_screen.h"
#include "ui_manager.h" // For ControllerAction enum

namespace d2::ui {

// Template method implementations
template<typename ActionType, typename ItemType>
bool MenuScreen<ActionType, ItemType>::handleControllerInput(d2::ControllerAction action) {
    switch (action) {
        case d2::ControllerAction::NAVIGATE_UP:
            navigateToPrevious();
            return true;
            
        case d2::ControllerAction::NAVIGATE_DOWN:
            navigateToNext();
            return true;
            
        case d2::ControllerAction::NAVIGATE_LEFT:
            navigateToPrevious();
            return true;
            
        case d2::ControllerAction::NAVIGATE_RIGHT:
            navigateToNext();
            return true;
            
        case d2::ControllerAction::ACTION_BUTTON:
            activateSelected();
            return true;
            
        case d2::ControllerAction::CANCEL_BUTTON:
            // Let derived classes handle cancel button
            return false;
            
        default:
            return false;
    }
}

} // namespace d2::ui