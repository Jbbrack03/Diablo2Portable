#include <gtest/gtest.h>
#include <memory>
#include "ui/pause_screen.h"

namespace d2 {

class PauseScreenTest : public ::testing::Test {
protected:
    void SetUp() override {
        pause_screen_ = std::make_unique<PauseScreen>();
    }
    
    std::unique_ptr<PauseScreen> pause_screen_;
};

TEST_F(PauseScreenTest, CreatePauseScreen) {
    // PauseScreen should be created successfully
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 0); // Default selection on first button
    EXPECT_EQ(pause_screen_->getButtonCount(), 5); // Character, Inventory, Skills, Options, Resume
    EXPECT_EQ(pause_screen_->getSelectedButtonText(), "Character");
}

TEST_F(PauseScreenTest, ButtonNavigation) {
    // Should start with first button selected
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 0);
    EXPECT_EQ(pause_screen_->getSelectedButtonText(), "Character");
    
    // Navigate down to next button
    pause_screen_->selectButton(1);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 1);
    EXPECT_EQ(pause_screen_->getSelectedButtonText(), "Inventory");
    
    // Navigate to Skills
    pause_screen_->selectButton(2);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 2);
    EXPECT_EQ(pause_screen_->getSelectedButtonText(), "Skills");
    
    // Navigate to Options
    pause_screen_->selectButton(3);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 3);
    EXPECT_EQ(pause_screen_->getSelectedButtonText(), "Options");
    
    // Navigate to Resume
    pause_screen_->selectButton(4);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 4);
    EXPECT_EQ(pause_screen_->getSelectedButtonText(), "Resume");
}

TEST_F(PauseScreenTest, HandleControllerNavigation) {
    // Should start with first button selected
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 0);
    
    // Navigate down should move to next button
    bool handled = pause_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 1);
    
    // Navigate down through all buttons
    handled = pause_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 2);
    
    handled = pause_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 3);
    
    handled = pause_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 4);
    
    // Navigate down at end should wrap to beginning
    handled = pause_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 0);
    
    // Navigate up should move to previous button (with wraparound)
    handled = pause_screen_->handleControllerInput(ControllerAction::NAVIGATE_UP);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 4);
}

TEST_F(PauseScreenTest, HandleButtonSelection) {
    // Select Character button
    pause_screen_->selectButton(0);
    bool handled = pause_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedAction(), PauseMenuAction::CHARACTER);
    
    // Select Inventory button
    pause_screen_->selectButton(1);
    handled = pause_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedAction(), PauseMenuAction::INVENTORY);
    
    // Select Skills button
    pause_screen_->selectButton(2);
    handled = pause_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedAction(), PauseMenuAction::SKILLS);
    
    // Select Options button
    pause_screen_->selectButton(3);
    handled = pause_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedAction(), PauseMenuAction::OPTIONS);
    
    // Select Resume button
    pause_screen_->selectButton(4);
    handled = pause_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedAction(), PauseMenuAction::RESUME);
}

TEST_F(PauseScreenTest, HandleCancelButton) {
    // Cancel button should trigger resume action
    bool handled = pause_screen_->handleControllerInput(ControllerAction::CANCEL_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(pause_screen_->getSelectedAction(), PauseMenuAction::RESUME);
}

TEST_F(PauseScreenTest, ResetSelection) {
    // Navigate to different button and select action
    pause_screen_->selectButton(2);
    pause_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_EQ(pause_screen_->getSelectedAction(), PauseMenuAction::SKILLS);
    
    // Reset should clear selected action
    pause_screen_->resetSelection();
    EXPECT_EQ(pause_screen_->getSelectedAction(), PauseMenuAction::NONE);
    EXPECT_EQ(pause_screen_->getSelectedButtonIndex(), 0); // Reset to first button
}

TEST_F(PauseScreenTest, GetButtonByIndex) {
    // Should be able to get button text by index
    EXPECT_EQ(pause_screen_->getButtonText(0), "Character");
    EXPECT_EQ(pause_screen_->getButtonText(1), "Inventory");
    EXPECT_EQ(pause_screen_->getButtonText(2), "Skills");
    EXPECT_EQ(pause_screen_->getButtonText(3), "Options");
    EXPECT_EQ(pause_screen_->getButtonText(4), "Resume");
    
    // Invalid index should return empty string
    EXPECT_EQ(pause_screen_->getButtonText(99), "");
}

} // namespace d2