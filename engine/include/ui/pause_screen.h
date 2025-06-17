#pragma once

#include "ui/ui_element.h"
#include "ui/ui_manager.h"
#include <string>
#include <vector>

namespace d2 {

enum class PauseMenuAction {
    NONE,
    CHARACTER,
    INVENTORY,
    SKILLS,
    OPTIONS,
    RESUME
};

struct PauseMenuButton {
    std::string text;
    PauseMenuAction action;
    
    PauseMenuButton(const std::string& text, PauseMenuAction action)
        : text(text), action(action) {}
};

class PauseScreen : public UIElement {
public:
    PauseScreen();
    virtual ~PauseScreen() = default;
    
    // Button management
    size_t getButtonCount() const;
    std::string getButtonText(size_t index) const;
    
    // Selection management
    void selectButton(size_t index);
    size_t getSelectedButtonIndex() const { return selected_button_index_; }
    std::string getSelectedButtonText() const;
    
    // Action handling
    PauseMenuAction getSelectedAction() const { return selected_action_; }
    void resetSelection();
    
    // Controller input
    bool handleControllerInput(ControllerAction action);
    
private:
    std::vector<PauseMenuButton> buttons_;
    size_t selected_button_index_{0};
    PauseMenuAction selected_action_{PauseMenuAction::NONE};
    
    // Helper methods
    void navigateToNextButton();
    void navigateToPreviousButton();
    void activateSelectedButton();
    void initializeButtons();
};

} // namespace d2