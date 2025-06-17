#include <gtest/gtest.h>
#include <memory>
#include "ui/main_menu_screen.h"

namespace d2 {

class MainMenuScreenTest : public ::testing::Test {
protected:
    void SetUp() override {
        main_menu_ = std::make_unique<MainMenuScreen>();
    }
    
    std::unique_ptr<MainMenuScreen> main_menu_;
};

TEST_F(MainMenuScreenTest, CreateMainMenuScreen) {
    // MainMenuScreen should be created successfully
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 0); // Default selection on first button
    EXPECT_EQ(main_menu_->getButtonCount(), 4); // New Game, Load Game, Options, Exit
    EXPECT_EQ(main_menu_->getSelectedButtonText(), "New Game");
}

TEST_F(MainMenuScreenTest, ButtonNavigation) {
    // Should start with first button selected
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 0);
    EXPECT_EQ(main_menu_->getSelectedButtonText(), "New Game");
    
    // Navigate down to next button
    main_menu_->selectButton(1);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 1);
    EXPECT_EQ(main_menu_->getSelectedButtonText(), "Load Game");
    
    // Navigate to Options
    main_menu_->selectButton(2);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 2);
    EXPECT_EQ(main_menu_->getSelectedButtonText(), "Options");
    
    // Navigate to Exit
    main_menu_->selectButton(3);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 3);
    EXPECT_EQ(main_menu_->getSelectedButtonText(), "Exit");
}

TEST_F(MainMenuScreenTest, HandleControllerNavigation) {
    // Should start with first button selected
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 0);
    
    // Navigate down should move to next button
    bool handled = main_menu_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 1);
    
    // Navigate down again
    handled = main_menu_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 2);
    
    // Navigate down again
    handled = main_menu_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 3);
    
    // Navigate down at end should wrap to beginning
    handled = main_menu_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 0);
    
    // Navigate up should move to previous button (with wraparound)
    handled = main_menu_->handleControllerInput(ControllerAction::NAVIGATE_UP);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 3);
}

TEST_F(MainMenuScreenTest, HandleButtonSelection) {
    // Select New Game button
    main_menu_->selectButton(0);
    bool handled = main_menu_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedAction(), MainMenuAction::NEW_GAME);
    
    // Select Load Game button
    main_menu_->selectButton(1);
    handled = main_menu_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedAction(), MainMenuAction::LOAD_GAME);
    
    // Select Options button
    main_menu_->selectButton(2);
    handled = main_menu_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedAction(), MainMenuAction::OPTIONS);
    
    // Select Exit button
    main_menu_->selectButton(3);
    handled = main_menu_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(main_menu_->getSelectedAction(), MainMenuAction::EXIT);
}

TEST_F(MainMenuScreenTest, ResetSelection) {
    // Navigate to different button and select action
    main_menu_->selectButton(2);
    main_menu_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_EQ(main_menu_->getSelectedAction(), MainMenuAction::OPTIONS);
    
    // Reset should clear selected action
    main_menu_->resetSelection();
    EXPECT_EQ(main_menu_->getSelectedAction(), MainMenuAction::NONE);
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 0); // Reset to first button
}

TEST_F(MainMenuScreenTest, GetButtonByIndex) {
    // Should be able to get button text by index
    EXPECT_EQ(main_menu_->getButtonText(0), "New Game");
    EXPECT_EQ(main_menu_->getButtonText(1), "Load Game");
    EXPECT_EQ(main_menu_->getButtonText(2), "Options");
    EXPECT_EQ(main_menu_->getButtonText(3), "Exit");
    
    // Invalid index should return empty string
    EXPECT_EQ(main_menu_->getButtonText(99), "");
}

TEST_F(MainMenuScreenTest, DisableEnableButtons) {
    // All buttons should be enabled by default
    EXPECT_TRUE(main_menu_->isButtonEnabled(0));  // New Game
    EXPECT_TRUE(main_menu_->isButtonEnabled(1));  // Load Game
    EXPECT_TRUE(main_menu_->isButtonEnabled(2));  // Options
    EXPECT_TRUE(main_menu_->isButtonEnabled(3));  // Exit
    
    // Should be able to disable Load Game if no save files exist
    main_menu_->setButtonEnabled(1, false);
    EXPECT_FALSE(main_menu_->isButtonEnabled(1));
    
    // Disabled button should be skipped during navigation
    main_menu_->selectButton(0); // Start at New Game
    main_menu_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    // Should skip disabled Load Game and go to Options
    EXPECT_EQ(main_menu_->getSelectedButtonIndex(), 2);
    EXPECT_EQ(main_menu_->getSelectedButtonText(), "Options");
}

} // namespace d2