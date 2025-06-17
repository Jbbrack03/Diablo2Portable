#include <gtest/gtest.h>
#include <memory>
#include "ui/character_screen.h"
#include "game/character.h"

namespace d2 {

using namespace d2::game;

class CharacterScreenTest : public ::testing::Test {
protected:
    void SetUp() override {
        character_ = std::make_unique<Character>(CharacterClass::BARBARIAN);
        character_screen_ = std::make_unique<CharacterScreen>(character_.get());
    }
    
    std::unique_ptr<Character> character_;
    std::unique_ptr<CharacterScreen> character_screen_;
};

TEST_F(CharacterScreenTest, CreateCharacterScreen) {
    // CharacterScreen should inherit from UIElement
    EXPECT_TRUE(character_screen_->isVisible());
    EXPECT_TRUE(character_screen_->isEnabled());
    
    // Should display character information
    EXPECT_EQ(character_screen_->getCharacterClass(), CharacterClass::BARBARIAN);
    EXPECT_EQ(character_screen_->getCharacterLevel(), 1);
}

TEST_F(CharacterScreenTest, DisplayCharacterStats) {
    // Set up some character stats
    character_->setLevel(10);
    character_->setStatPoints(5);
    
    // Screen should reflect current character stats
    EXPECT_EQ(character_screen_->getDisplayedLevel(), 10);
    EXPECT_EQ(character_screen_->getAvailableStatPoints(), 5);
    
    // Should show base stats
    EXPECT_GT(character_screen_->getDisplayedStrength(), 0);
    EXPECT_GT(character_screen_->getDisplayedDexterity(), 0);
    EXPECT_GT(character_screen_->getDisplayedVitality(), 0);
    EXPECT_GT(character_screen_->getDisplayedEnergy(), 0);
}

TEST_F(CharacterScreenTest, AllocateStatPoints) {
    character_->setLevel(5);
    character_->setStatPoints(10);
    
    int initialStr = character_->getStrength();
    
    // Select strength stat for increase
    character_screen_->selectStat(StatType::STRENGTH);
    EXPECT_EQ(character_screen_->getSelectedStat(), StatType::STRENGTH);
    
    // Allocate a stat point
    bool allocated = character_screen_->allocateStatPoint();
    EXPECT_TRUE(allocated);
    
    // Should increase character strength and decrease stat points
    EXPECT_EQ(character_->getStrength(), initialStr + 1);
    EXPECT_EQ(character_->getStatPoints(), 9);
}

TEST_F(CharacterScreenTest, PreventInvalidStatAllocation) {
    character_->setStatPoints(0);
    
    // Try to allocate when no points available
    character_screen_->selectStat(StatType::VITALITY);
    bool allocated = character_screen_->allocateStatPoint();
    EXPECT_FALSE(allocated);
    
    // Character stats should remain unchanged
    EXPECT_EQ(character_->getStatPoints(), 0);
}

TEST_F(CharacterScreenTest, DisplayDerivedStats) {
    character_->setLevel(10);
    
    // Screen should display derived stats
    EXPECT_GT(character_screen_->getDisplayedLife(), 0);
    EXPECT_GT(character_screen_->getDisplayedMana(), 0);
    EXPECT_GT(character_screen_->getDisplayedStamina(), 0);
    
    // Should reflect character class bonuses
    if (character_->getCharacterClass() == CharacterClass::BARBARIAN) {
        EXPECT_GT(character_screen_->getDisplayedLife(), 100); // Barbarians have high life
    }
}

TEST_F(CharacterScreenTest, HandleControllerNavigation) {
    // Start with no stat selected
    EXPECT_EQ(character_screen_->getSelectedStat(), StatType::STRENGTH); // Default selection
    
    // Navigate through stats
    character_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_EQ(character_screen_->getSelectedStat(), StatType::DEXTERITY);
    
    character_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_EQ(character_screen_->getSelectedStat(), StatType::VITALITY);
    
    character_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_EQ(character_screen_->getSelectedStat(), StatType::ENERGY);
    
    // Navigate up
    character_screen_->handleControllerInput(ControllerAction::NAVIGATE_UP);
    EXPECT_EQ(character_screen_->getSelectedStat(), StatType::VITALITY);
}

TEST_F(CharacterScreenTest, AllocateWithActionButton) {
    character_->setStatPoints(5);
    character_screen_->selectStat(StatType::DEXTERITY);
    
    int initialDex = character_->getDexterity();
    
    // Use action button to allocate point
    character_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    
    EXPECT_EQ(character_->getDexterity(), initialDex + 1);
    EXPECT_EQ(character_->getStatPoints(), 4);
}

} // namespace d2