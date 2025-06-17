#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <stack>
#include "ui_element.h"

namespace d2 {

enum class ControllerAction {
    NAVIGATE_UP,
    NAVIGATE_DOWN,
    NAVIGATE_LEFT,
    NAVIGATE_RIGHT,
    ACTION_BUTTON,
    CANCEL_BUTTON
};

class UIManager {
public:
    UIManager() = default;
    ~UIManager() = default;

    // Screen management
    void pushScreen(const std::string& name, std::shared_ptr<UIElement> screen);
    void popScreen();
    void switchToScreen(const std::string& name);
    void addScreen(const std::string& name, std::shared_ptr<UIElement> screen);
    
    // Screen queries
    bool hasActiveScreen() const;
    std::string getActiveScreenName() const;
    
    // Input handling
    bool handleControllerInput(ControllerAction action);
    
private:
    std::unordered_map<std::string, std::shared_ptr<UIElement>> screens_;
    std::stack<std::string> screen_stack_;
};

} // namespace d2