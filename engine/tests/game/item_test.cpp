#include <gtest/gtest.h>
#include "game/item.h"
#include "game/character.h"  // For StatType enum

using namespace d2::game;

class ItemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup will go here
    }
};

// Test for Phase 4, Task 4.3: Item System - Basic item creation
TEST_F(ItemTest, BasicItemCreation) {
    // Create a basic weapon item
    Item sword("Short Sword", ItemType::WEAPON);
    
    EXPECT_EQ(sword.getName(), "Short Sword");
    EXPECT_EQ(sword.getType(), ItemType::WEAPON);
    EXPECT_EQ(sword.getRequiredLevel(), 1);  // Default level requirement
    
    // Create armor item
    Item armor("Leather Armor", ItemType::ARMOR);
    EXPECT_EQ(armor.getName(), "Leather Armor");
    EXPECT_EQ(armor.getType(), ItemType::ARMOR);
}

// Test for Phase 4, Task 4.3: Item System - Stat modifiers
TEST_F(ItemTest, ItemStatModifiers) {
    Item sword("Long Sword", ItemType::WEAPON);
    
    // Set weapon damage
    sword.setDamage(10, 15);  // min-max damage
    EXPECT_EQ(sword.getMinDamage(), 10);
    EXPECT_EQ(sword.getMaxDamage(), 15);
    
    // Set stat bonuses
    sword.addStatBonus(StatType::STRENGTH, 5);
    sword.addStatBonus(StatType::DEXTERITY, 3);
    
    EXPECT_EQ(sword.getStatBonus(StatType::STRENGTH), 5);
    EXPECT_EQ(sword.getStatBonus(StatType::DEXTERITY), 3);
    EXPECT_EQ(sword.getStatBonus(StatType::VITALITY), 0);  // No bonus
    
    // Test armor with defense
    Item armor("Chain Mail", ItemType::ARMOR);
    armor.setDefense(45);
    EXPECT_EQ(armor.getDefense(), 45);
}

// Test for Phase 4, Task 4.3: Item System - Rarity system
TEST_F(ItemTest, ItemRaritySystem) {
    // Test normal quality item (gray)
    Item normalSword("Short Sword", ItemType::WEAPON);
    EXPECT_EQ(normalSword.getRarity(), ItemRarity::NORMAL);
    EXPECT_EQ(normalSword.getMaxAffixes(), 0);  // Normal items have no affixes
    
    // Test magic quality item (blue)
    Item magicSword("Long Sword", ItemType::WEAPON);
    magicSword.setRarity(ItemRarity::MAGIC);
    EXPECT_EQ(magicSword.getRarity(), ItemRarity::MAGIC);
    EXPECT_EQ(magicSword.getMaxAffixes(), 2);  // Magic items can have 1-2 affixes
    
    // Test rare quality item (yellow)
    Item rareHelm("War Helm", ItemType::ARMOR);
    rareHelm.setRarity(ItemRarity::RARE);
    EXPECT_EQ(rareHelm.getRarity(), ItemRarity::RARE);
    EXPECT_EQ(rareHelm.getMaxAffixes(), 6);  // Rare items can have 3-6 affixes
    
    // Test unique quality item (gold)
    Item uniqueRing("Stone of Jordan", ItemType::ACCESSORY);
    uniqueRing.setRarity(ItemRarity::UNIQUE);
    EXPECT_EQ(uniqueRing.getRarity(), ItemRarity::UNIQUE);
    EXPECT_TRUE(uniqueRing.hasFixedStats());  // Unique items have predetermined stats
}