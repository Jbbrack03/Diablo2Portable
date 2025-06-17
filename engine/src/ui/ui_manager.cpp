#include "ui/ui_manager.h"

namespace d2 {

void UIManager::pushScreen(const std::string& name, std::shared_ptr<UIElement> screen) {
    screens_[name] = screen;
    screen_stack_.push(name);
}

void UIManager::popScreen() {
    if (!screen_stack_.empty()) {
        screen_stack_.pop();
    }
}

void UIManager::switchToScreen(const std::string& name) {
    // Clear current stack
    while (!screen_stack_.empty()) {
        screen_stack_.pop();
    }
    
    // Push new screen
    if (screens_.find(name) != screens_.end()) {
        screen_stack_.push(name);
    }
}

void UIManager::addScreen(const std::string& name, std::shared_ptr<UIElement> screen) {
    screens_[name] = screen;
}

bool UIManager::hasActiveScreen() const {
    return !screen_stack_.empty();
}

std::string UIManager::getActiveScreenName() const {
    if (screen_stack_.empty()) {
        return "";
    }
    return screen_stack_.top();
}

bool UIManager::handleControllerInput(ControllerAction action) {
    // For now, just return true to indicate input was handled
    // Future implementation will route input to active screen
    return true;
}

} // namespace d2