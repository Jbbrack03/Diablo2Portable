#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include "ui/menu_screen_impl.h"

// Use the actual ControllerAction enum from the UI system
using ControllerAction = d2::ControllerAction;

// Test-specific enums
enum class MenuAction {
    NONE,
    START_GAME,
    SETTINGS,
    EXIT
};

struct MenuItem {
    std::string text;
    MenuAction action;
    bool enabled;
    
    MenuItem(const std::string& t, MenuAction a, bool e = true) 
        : text(t), action(a), enabled(e) {}
};

// Concrete test implementation
class TestMenuScreen : public d2::ui::MenuScreen<MenuAction, MenuItem> {
public:
    TestMenuScreen() {
        addItem(MenuItem("Start Game", MenuAction::START_GAME));
        addItem(MenuItem("Settings", MenuAction::SETTINGS));
        addItem(MenuItem("Exit", MenuAction::EXIT));
    }
    
    // Track activation for testing
    size_t last_activated_index_{SIZE_MAX};
    bool activation_called_{false};
    
protected:
    bool isItemEnabled(size_t index) const override {
        if (index >= items_.size()) return false;
        return items_[index].enabled;
    }
    
    void onItemActivated(size_t index) override {
        last_activated_index_ = index;
        activation_called_ = true;
        if (index < items_.size()) {
            selected_action_ = items_[index].action;
        }
    }
};

class MenuScreenRefactorTest : public ::testing::Test {
protected:
    void SetUp() override {
        menu_screen = std::make_unique<TestMenuScreen>();
    }
    
    std::unique_ptr<TestMenuScreen> menu_screen;
};

TEST_F(MenuScreenRefactorTest, InitializesWithValidState) {
    EXPECT_EQ(menu_screen->getSelectedIndex(), 0);
    EXPECT_EQ(menu_screen->getSelectedAction(), MenuAction::NONE);
    EXPECT_EQ(menu_screen->getItemCount(), 3);
}

TEST_F(MenuScreenRefactorTest, NavigationMovesSelection) {
    EXPECT_EQ(menu_screen->getSelectedIndex(), 0);
    
    menu_screen->navigateToNext();
    EXPECT_EQ(menu_screen->getSelectedIndex(), 1);
    
    menu_screen->navigateToNext();
    EXPECT_EQ(menu_screen->getSelectedIndex(), 2);
    
    menu_screen->navigateToPrevious();
    EXPECT_EQ(menu_screen->getSelectedIndex(), 1);
}

TEST_F(MenuScreenRefactorTest, NavigationWrapsAround) {
    // Test wrap-around at end
    menu_screen->selectItem(2); // Move to last item
    menu_screen->navigateToNext();
    EXPECT_EQ(menu_screen->getSelectedIndex(), 0); // Should wrap to first
    
    // Test wrap-around at beginning
    menu_screen->selectItem(0); // Move to first item
    menu_screen->navigateToPrevious();
    EXPECT_EQ(menu_screen->getSelectedIndex(), 2); // Should wrap to last
}

TEST_F(MenuScreenRefactorTest, SelectItemValidatesIndex) {
    menu_screen->selectItem(1);
    EXPECT_EQ(menu_screen->getSelectedIndex(), 1);
    
    // Test invalid index - should not change selection
    menu_screen->selectItem(999);
    EXPECT_EQ(menu_screen->getSelectedIndex(), 1);
}

TEST_F(MenuScreenRefactorTest, ActivateSelectedCallsOnItemActivated) {
    menu_screen->selectItem(1);
    menu_screen->activateSelected();
    
    EXPECT_TRUE(menu_screen->activation_called_);
    EXPECT_EQ(menu_screen->last_activated_index_, 1);
    EXPECT_EQ(menu_screen->getSelectedAction(), MenuAction::SETTINGS);
}

TEST_F(MenuScreenRefactorTest, HandleControllerInputNavigatesCorrectly) {
    EXPECT_EQ(menu_screen->getSelectedIndex(), 0);
    
    bool handled = menu_screen->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(menu_screen->getSelectedIndex(), 1);
    
    handled = menu_screen->handleControllerInput(ControllerAction::NAVIGATE_UP);
    EXPECT_TRUE(handled);
    EXPECT_EQ(menu_screen->getSelectedIndex(), 0);
}

TEST_F(MenuScreenRefactorTest, HandleControllerInputActivatesSelected) {
    menu_screen->selectItem(2);
    
    bool handled = menu_screen->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_TRUE(menu_screen->activation_called_);
    EXPECT_EQ(menu_screen->last_activated_index_, 2);
    EXPECT_EQ(menu_screen->getSelectedAction(), MenuAction::EXIT);
}

TEST_F(MenuScreenRefactorTest, SkipsDisabledItems) {
    // Add a disabled item
    menu_screen->addItem(MenuItem("Disabled", MenuAction::NONE, false));
    
    // Navigate to the disabled item
    menu_screen->selectItem(2); // Exit
    menu_screen->navigateToNext(); // Should skip disabled item and wrap to Start
    
    EXPECT_EQ(menu_screen->getSelectedIndex(), 0);
}

TEST_F(MenuScreenRefactorTest, HandleControllerInputReturnsFalseForUnknownAction) {
    bool handled = menu_screen->handleControllerInput(ControllerAction::CANCEL_BUTTON);
    EXPECT_FALSE(handled);
}

TEST_F(MenuScreenRefactorTest, PreservesExistingFunctionality) {
    // Test that all basic menu operations work as expected
    EXPECT_EQ(menu_screen->getItemCount(), 3);
    
    // Test item access
    const auto& item = menu_screen->getItem(0);
    EXPECT_EQ(item.text, "Start Game");
    EXPECT_EQ(item.action, MenuAction::START_GAME);
    EXPECT_TRUE(item.enabled);
    
    // Test complete navigation cycle
    for (size_t i = 0; i < menu_screen->getItemCount(); ++i) {
        menu_screen->selectItem(i);
        menu_screen->activateSelected();
        EXPECT_EQ(menu_screen->last_activated_index_, i);
    }
}