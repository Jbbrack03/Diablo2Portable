#include <gtest/gtest.h>
#include "game/character.h"
#include "game/item.h"

using namespace d2::game;

// Helper function to create test weapons
std::shared_ptr<Item> createTestWeapon(int minDamage, int maxDamage, int strengthRequirement) {
    auto weapon = std::make_shared<Item>("Test Sword", ItemType::WEAPON);
    weapon->setDamage(minDamage, maxDamage);
    weapon->setRequiredStrength(strengthRequirement);
    return weapon;
}

class CharacterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup will go here
    }
};

// Test for Phase 4, Task 4.1: Character System - Base stats calculation
TEST_F(CharacterTest, StatCalculation) {
    // Create a Barbarian character
    // D2 Barbarian starting stats:
    // STR: 30, DEX: 20, VIT: 25, ENE: 10
    // Starting Life: 55, Mana: 10
    Character barbarian(CharacterClass::BARBARIAN);
    barbarian.setLevel(10);
    barbarian.addStatPoint(StatType::STRENGTH, 20);
    
    // Test base damage calculation
    // Current implementation: Base damage + STR bonus
    // TODO: In actual D2, STR provides 1% enhanced damage per point
    // This would need weapon damage to calculate properly
    EXPECT_EQ(barbarian.getBaseDamage(), 35); // Current formula: 15 + (50 STR / 2.5) = 35
    
    // Test life calculation
    // D2 Formula for Barbarian: 
    // - Base life: 55
    // - Life per level: 2
    // - Life per vitality: 4 (corrected from 14)
    // Expected with accurate formula: 55 + 9*2 + 25*4 = 55 + 18 + 100 = 173
    EXPECT_EQ(barbarian.getLife(), 173); // Now using correct D2 formula
}

// Test for Phase 10, Task 10.1: Correct D2 Life Calculation (MUST FAIL FIRST)
TEST_F(CharacterTest, CorrectLifeCalculation) {
    Character barbarian(CharacterClass::BARBARIAN);
    barbarian.setLevel(10);
    barbarian.addStatPoint(StatType::VITALITY, 10); // 25 base + 10 = 35 total vitality
    
    // D2 Formula: Base Life + (Level - 1) * Life per Level + Vitality * Life per Vitality
    // Barbarian: 55 base + (10-1) * 2 + 35 * 4 = 55 + 18 + 140 = 213
    EXPECT_EQ(barbarian.getLife(), 213);
}

// Test for Phase 4, Task 4.1: Character System - Level up
TEST_F(CharacterTest, LevelUp) {
    // Create a Sorceress character
    // D2 Sorceress starting stats:
    // STR: 10, DEX: 25, VIT: 10, ENE: 35
    // Starting Life: 40, Mana: 35
    Character sorc(CharacterClass::SORCERESS);
    
    // Add enough experience to level up
    // D2 Experience requirements:
    // Level 1 → 2: 500 exp
    // Level 2 → 3: 1,500 exp (total)
    // Current implementation uses simplified 1000 exp threshold
    sorc.addExperience(1000);
    
    // Check level increased
    EXPECT_EQ(sorc.getLevel(), 2);
    
    // Check stat points awarded (5 per level in D2) ✓ CORRECT
    EXPECT_EQ(sorc.getStatPoints(), 5);
    
    // Check skill points awarded (1 per level in D2) ✓ CORRECT
    EXPECT_EQ(sorc.getSkillPoints(), 1);
}

// Test for Phase 10, Task 10.7: Strength Damage Bonus (MUST FAIL FIRST)
TEST_F(CharacterTest, StrengthDamageBonusCalculation) {
    Character character(CharacterClass::BARBARIAN);
    character.setStat(StatType::STRENGTH, 100); // 100 total strength
    
    // D2 Formula: 1% damage per strength point above required
    // But actually, ALL strength contributes, not just above requirement
    auto weapon = createTestWeapon(10, 20, 50); // 50 STR required
    character.equipWeapon(weapon);
    
    float expectedBonus = 100 * 0.01f; // 100% bonus (1.0)
    float actualBonus = character.getStrengthDamageBonus();
    
    EXPECT_NEAR(actualBonus, expectedBonus, 0.001f);
}

// Test for Phase 10, Task 10.7: Insufficient Strength No Damage Bonus
TEST_F(CharacterTest, InsufficientStrengthNoDamageBonus) {
    Character character(CharacterClass::SORCERESS);
    character.setStat(StatType::STRENGTH, 30); // Low strength
    
    auto weapon = createTestWeapon(10, 20, 50); // 50 STR required
    character.equipWeapon(weapon);
    
    // No bonus if strength requirement not met
    EXPECT_EQ(character.getStrengthDamageBonus(), 0.0f);
}