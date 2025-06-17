#pragma once

#include "ui/ui_element.h"
#include "ui/ui_manager.h"
#include <string>
#include <vector>

namespace d2 {

enum class MainMenuAction {
    NONE,
    NEW_GAME,
    LOAD_GAME,
    OPTIONS,
    EXIT
};

struct MenuButton {
    std::string text;
    bool enabled;
    MainMenuAction action;
    
    MenuButton(const std::string& text, MainMenuAction action)
        : text(text), enabled(true), action(action) {}
};

class MainMenuScreen : public UIElement {
public:
    MainMenuScreen();
    virtual ~MainMenuScreen() = default;
    
    // Button management
    size_t getButtonCount() const;
    std::string getButtonText(size_t index) const;
    bool isButtonEnabled(size_t index) const;
    void setButtonEnabled(size_t index, bool enabled);
    
    // Selection management
    void selectButton(size_t index);
    size_t getSelectedButtonIndex() const { return selected_button_index_; }
    std::string getSelectedButtonText() const;
    
    // Action handling
    MainMenuAction getSelectedAction() const { return selected_action_; }
    void resetSelection();
    
    // Controller input
    bool handleControllerInput(ControllerAction action);
    
private:
    std::vector<MenuButton> buttons_;
    size_t selected_button_index_{0};
    MainMenuAction selected_action_{MainMenuAction::NONE};
    
    // Helper methods
    void navigateToNextButton();
    void navigateToPreviousButton();
    void activateSelectedButton();
    void initializeButtons();
    size_t findNextEnabledButton(size_t start_index, bool forward) const;
};

} // namespace d2