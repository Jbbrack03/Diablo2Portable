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