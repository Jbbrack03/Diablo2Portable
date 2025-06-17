#include "ui/pause_screen.h"

namespace d2 {

PauseScreen::PauseScreen() {
    initializeButtons();
}

size_t PauseScreen::getButtonCount() const {
    return buttons_.size();
}

std::string PauseScreen::getButtonText(size_t index) const {
    if (index >= buttons_.size()) {
        return "";
    }
    return buttons_[index].text;
}

void PauseScreen::selectButton(size_t index) {
    if (index < buttons_.size()) {
        selected_button_index_ = index;
    }
}

std::string PauseScreen::getSelectedButtonText() const {
    if (selected_button_index_ >= buttons_.size()) {
        return "";
    }
    return buttons_[selected_button_index_].text;
}

void PauseScreen::resetSelection() {
    selected_action_ = PauseMenuAction::NONE;
    selected_button_index_ = 0;
}

bool PauseScreen::handleControllerInput(ControllerAction action) {
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
            // Cancel should resume the game
            selected_action_ = PauseMenuAction::RESUME;
            return true;
            
        default:
            return false;
    }
}

void PauseScreen::navigateToNextButton() {
    if (buttons_.empty()) {
        return;
    }
    
    selected_button_index_ = (selected_button_index_ + 1) % buttons_.size();
}

void PauseScreen::navigateToPreviousButton() {
    if (buttons_.empty()) {
        return;
    }
    
    if (selected_button_index_ == 0) {
        selected_button_index_ = buttons_.size() - 1;
    } else {
        selected_button_index_--;
    }
}

void PauseScreen::activateSelectedButton() {
    if (selected_button_index_ < buttons_.size()) {
        selected_action_ = buttons_[selected_button_index_].action;
    }
}

void PauseScreen::initializeButtons() {
    buttons_.clear();
    buttons_.emplace_back("Character", PauseMenuAction::CHARACTER);
    buttons_.emplace_back("Inventory", PauseMenuAction::INVENTORY);
    buttons_.emplace_back("Skills", PauseMenuAction::SKILLS);
    buttons_.emplace_back("Options", PauseMenuAction::OPTIONS);
    buttons_.emplace_back("Resume", PauseMenuAction::RESUME);
}

} // namespace d2