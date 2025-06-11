#include <gtest/gtest.h>
#include "game/character.h"

using namespace d2::game;

class CharacterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup will go here
    }
};

// Test for Phase 4, Task 4.1: Character System - Base stats calculation
TEST_F(CharacterTest, StatCalculation) {
    // Create a Barbarian character
    Character barbarian(CharacterClass::BARBARIAN);
    barbarian.setLevel(10);
    barbarian.addStatPoint(StatType::STRENGTH, 20);
    
    // Test base damage calculation (Base + STR bonus)
    EXPECT_EQ(barbarian.getBaseDamage(), 35); // Base + STR bonus
    
    // Test life calculation (Base + VIT + level)
    EXPECT_EQ(barbarian.getLife(), 420); // Base + VIT + level
}

// Test for Phase 4, Task 4.1: Character System - Level up
TEST_F(CharacterTest, LevelUp) {
    // Create a Sorceress character
    Character sorc(CharacterClass::SORCERESS);
    
    // Add enough experience to level up
    sorc.addExperience(1000);
    
    // Check level increased
    EXPECT_EQ(sorc.getLevel(), 2);
    
    // Check stat points awarded (5 per level in D2)
    EXPECT_EQ(sorc.getStatPoints(), 5);
    
    // Check skill points awarded (1 per level in D2)
    EXPECT_EQ(sorc.getSkillPoints(), 1);
}