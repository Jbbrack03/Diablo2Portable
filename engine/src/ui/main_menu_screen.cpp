#include "ui/main_menu_screen.h"

namespace d2 {

MainMenuScreen::MainMenuScreen() {
    initializeButtons();
}

size_t MainMenuScreen::getButtonCount() const {
    return buttons_.size();
}

std::string MainMenuScreen::getButtonText(size_t index) const {
    if (index >= buttons_.size()) {
        return "";
    }
    return buttons_[index].text;
}

bool MainMenuScreen::isButtonEnabled(size_t index) const {
    if (index >= buttons_.size()) {
        return false;
    }
    return buttons_[index].enabled;
}

void MainMenuScreen::setButtonEnabled(size_t index, bool enabled) {
    if (index < buttons_.size()) {
        buttons_[index].enabled = enabled;
        
        // If we disabled the currently selected button, move to next enabled one
        if (!enabled && selected_button_index_ == index) {
            size_t next_enabled = findNextEnabledButton(index, true);
            if (next_enabled != index) {
                selected_button_index_ = next_enabled;
            }
        }
    }
}

void MainMenuScreen::selectButton(size_t index) {
    if (index < buttons_.size() && buttons_[index].enabled) {
        selected_button_index_ = index;
    }
}

std::string MainMenuScreen::getSelectedButtonText() const {
    if (selected_button_index_ >= buttons_.size()) {
        return "";
    }
    return buttons_[selected_button_index_].text;
}

void MainMenuScreen::resetSelection() {
    selected_action_ = MainMenuAction::NONE;
    selected_button_index_ = 0;
}

bool MainMenuScreen::handleControllerInput(ControllerAction action) {
    switch (action) {
        case ControllerAction::NAVIGATE_UP:
            navigateToPreviousButton();
            return true;
            
        case ControllerAction::NAVIGATE_DOWN:
            navigateToNextButton();
            return true;
            
        case ControllerAction::ACTION_BUTTON:
            activateSelectedButton();
            return true;
            
        case ControllerAction::CANCEL_BUTTON:
            // Handle cancel (could close menu, etc.)
            return true;
            
        default:
            return false;
    }
}

void MainMenuScreen::navigateToNextButton() {
    if (buttons_.empty()) {
        return;
    }
    
    size_t next_index = findNextEnabledButton(selected_button_index_, true);
    selected_button_index_ = next_index;
}

void MainMenuScreen::navigateToPreviousButton() {
    if (buttons_.empty()) {
        return;
    }
    
    size_t prev_index = findNextEnabledButton(selected_button_index_, false);
    selected_button_index_ = prev_index;
}

void MainMenuScreen::activateSelectedButton() {
    if (selected_button_index_ < buttons_.size() && buttons_[selected_button_index_].enabled) {
        selected_action_ = buttons_[selected_button_index_].action;
    }
}

void MainMenuScreen::initializeButtons() {
    buttons_.clear();
    buttons_.emplace_back("New Game", MainMenuAction::NEW_GAME);
    buttons_.emplace_back("Load Game", MainMenuAction::LOAD_GAME);
    buttons_.emplace_back("Options", MainMenuAction::OPTIONS);
    buttons_.emplace_back("Exit", MainMenuAction::EXIT);
}

size_t MainMenuScreen::findNextEnabledButton(size_t start_index, bool forward) const {
    if (buttons_.empty()) {
        return 0;
    }
    
    size_t current = start_index;
    size_t count = 0;
    
    do {
        if (forward) {
            current = (current + 1) % buttons_.size();
        } else {
            current = (current == 0) ? buttons_.size() - 1 : current - 1;
        }
        count++;
        
        if (buttons_[current].enabled) {
            return current;
        }
        
    } while (count < buttons_.size());
    
    // If no enabled buttons found, return the start index
    return start_index;
}

} // namespace d2