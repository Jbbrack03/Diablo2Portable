#include <gtest/gtest.h>
#include <memory>
#include "ui/ui_manager.h"
#include "ui/ui_element.h"

namespace d2 {

class MockUIScreen : public UIElement {
public:
    MockUIScreen(const std::string& name) : name_(name) {}
    const std::string& getName() const { return name_; }
    
private:
    std::string name_;
};

class UIManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        ui_manager_ = std::make_unique<UIManager>();
    }
    
    std::unique_ptr<UIManager> ui_manager_;
};

TEST_F(UIManagerTest, CreateUIManager) {
    // UIManager should be created successfully
    EXPECT_FALSE(ui_manager_->hasActiveScreen());
    EXPECT_EQ(ui_manager_->getActiveScreenName(), "");
}

TEST_F(UIManagerTest, PushAndPopScreens) {
    auto main_menu = std::make_shared<MockUIScreen>("MainMenu");
    auto inventory = std::make_shared<MockUIScreen>("Inventory");
    
    // Push main menu screen
    ui_manager_->pushScreen("MainMenu", main_menu);
    EXPECT_TRUE(ui_manager_->hasActiveScreen());
    EXPECT_EQ(ui_manager_->getActiveScreenName(), "MainMenu");
    
    // Push inventory screen on top
    ui_manager_->pushScreen("Inventory", inventory);
    EXPECT_TRUE(ui_manager_->hasActiveScreen());
    EXPECT_EQ(ui_manager_->getActiveScreenName(), "Inventory");
    
    // Pop inventory, should return to main menu
    ui_manager_->popScreen();
    EXPECT_TRUE(ui_manager_->hasActiveScreen());
    EXPECT_EQ(ui_manager_->getActiveScreenName(), "MainMenu");
    
    // Pop main menu, should have no active screen
    ui_manager_->popScreen();
    EXPECT_FALSE(ui_manager_->hasActiveScreen());
}

TEST_F(UIManagerTest, SwitchToScreen) {
    auto main_menu = std::make_shared<MockUIScreen>("MainMenu");
    auto character_screen = std::make_shared<MockUIScreen>("Character");
    
    ui_manager_->pushScreen("MainMenu", main_menu);
    ui_manager_->addScreen("Character", character_screen);
    
    // Switch to character screen (should clear stack and push new screen)
    ui_manager_->switchToScreen("Character");
    EXPECT_TRUE(ui_manager_->hasActiveScreen());
    EXPECT_EQ(ui_manager_->getActiveScreenName(), "Character");
    
    // Should only have one screen in stack
    ui_manager_->popScreen();
    EXPECT_FALSE(ui_manager_->hasActiveScreen());
}

TEST_F(UIManagerTest, HandleControllerNavigation) {
    auto screen = std::make_shared<MockUIScreen>("TestScreen");
    ui_manager_->pushScreen("TestScreen", screen);
    
    // Simulate controller input
    bool handled = ui_manager_->handleControllerInput(ControllerAction::NAVIGATE_UP);
    EXPECT_TRUE(handled); // Should handle navigation even if no focusable elements
    
    handled = ui_manager_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled); // Should handle action button
    
    handled = ui_manager_->handleControllerInput(ControllerAction::CANCEL_BUTTON);
    EXPECT_TRUE(handled); // Should handle cancel button
}

} // namespace d2