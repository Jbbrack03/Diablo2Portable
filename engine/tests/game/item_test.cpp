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

// Test for Phase 4, Task 4.3: Item System - Prefix generation for magic items
TEST_F(ItemTest, ItemPrefixGeneration) {
    // Create a magic weapon with a prefix
    Item magicSword("Long Sword", ItemType::WEAPON);
    magicSword.setRarity(ItemRarity::MAGIC);
    
    // Generate a prefix - "Sharp" adds enhanced damage
    magicSword.generatePrefix(1);  // Seed for deterministic testing
    
    // Verify the prefix was applied
    EXPECT_TRUE(magicSword.hasPrefix());
    EXPECT_EQ(magicSword.getPrefixName(), "Sharp");
    
    // Sharp prefix should add 20% enhanced damage
    // Base damage 10-15 becomes 12-18
    magicSword.setDamage(10, 15);
    EXPECT_EQ(magicSword.getMinDamage(), 12);  // 10 + 20% = 12
    EXPECT_EQ(magicSword.getMaxDamage(), 18);  // 15 + 20% = 18
    
    // The full item name should include the prefix
    EXPECT_EQ(magicSword.getFullName(), "Sharp Long Sword");
}

// Test for Phase 4, Task 4.3: Item System - Suffix generation for magic items
TEST_F(ItemTest, ItemSuffixGeneration) {
    // Create a magic armor with a suffix
    Item magicArmor("Ring Mail", ItemType::ARMOR);
    magicArmor.setRarity(ItemRarity::MAGIC);
    
    // Generate a suffix - "of the Fox" adds dexterity
    magicArmor.generateSuffix(1);  // Seed for deterministic testing
    
    // Verify the suffix was applied
    EXPECT_TRUE(magicArmor.hasSuffix());
    EXPECT_EQ(magicArmor.getSuffixName(), "of the Fox");
    
    // "of the Fox" suffix should add +5 dexterity
    EXPECT_EQ(magicArmor.getStatBonus(StatType::DEXTERITY), 5);
    
    // The full item name should include the suffix
    EXPECT_EQ(magicArmor.getFullName(), "Ring Mail of the Fox");
}

// Test for Phase 4, Task 4.3: Item System - Affix pool based on item level
TEST_F(ItemTest, ItemLevelBasedAffixPool) {
    // Low level item can only get basic affixes
    Item lowLevelSword("Short Sword", ItemType::WEAPON);
    lowLevelSword.setRarity(ItemRarity::MAGIC);
    lowLevelSword.setItemLevel(5);  // Low level item
    
    // Generate prefix with seed that would give "Cruel" on high level items
    lowLevelSword.generatePrefix(2);  // Seed 2 for different selection
    
    // Low level items can't get "Cruel" prefix (requires level 35+)
    // Should get "Heavy" instead (low level prefix)
    EXPECT_EQ(lowLevelSword.getPrefixName(), "Heavy");
    
    // Heavy adds 5-10 damage
    lowLevelSword.setDamage(10, 15);
    EXPECT_EQ(lowLevelSword.getMinDamage(), 15);  // 10 + 5
    EXPECT_EQ(lowLevelSword.getMaxDamage(), 25);  // 15 + 10
    
    // High level item can get better affixes
    Item highLevelSword("Colossus Blade", ItemType::WEAPON);
    highLevelSword.setRarity(ItemRarity::MAGIC);
    highLevelSword.setItemLevel(50);  // High level item
    
    // Same seed now gives "Cruel" prefix on high level item
    highLevelSword.generatePrefix(2);
    EXPECT_EQ(highLevelSword.getPrefixName(), "Cruel");
    
    // Cruel adds 200% enhanced damage (much stronger)
    highLevelSword.setDamage(50, 60);
    EXPECT_EQ(highLevelSword.getMinDamage(), 150);  // 50 * 3
    EXPECT_EQ(highLevelSword.getMaxDamage(), 180);  // 60 * 3
}

// Test for Phase 4, Task 4.3: Item System - Affix value ranges (random rolls)
TEST_F(ItemTest, ItemAffixValueRanges) {
    // Create an armor with a suffix that has variable values
    Item magicGloves("Leather Gloves", ItemType::ARMOR);
    magicGloves.setRarity(ItemRarity::MAGIC);
    
    // Generate "of Strength" suffix with different roll seeds
    // Seed includes both affix selection and value roll
    magicGloves.generateSuffix(10);  // Seed 10 = "of Strength" with min roll
    
    EXPECT_EQ(magicGloves.getSuffixName(), "of Strength");
    // "of Strength" can roll 3-7 strength, seed 10 gives minimum
    EXPECT_EQ(magicGloves.getStatBonus(StatType::STRENGTH), 3);
    
    // Create another item with max roll
    Item magicGloves2("Leather Gloves", ItemType::ARMOR);
    magicGloves2.setRarity(ItemRarity::MAGIC);
    magicGloves2.generateSuffix(11);  // Seed 11 = "of Strength" with max roll
    
    EXPECT_EQ(magicGloves2.getSuffixName(), "of Strength");
    // Same suffix but max roll
    EXPECT_EQ(magicGloves2.getStatBonus(StatType::STRENGTH), 7);
    
    // Test mid-range roll
    Item magicGloves3("Leather Gloves", ItemType::ARMOR);
    magicGloves3.setRarity(ItemRarity::MAGIC);
    magicGloves3.generateSuffix(12);  // Seed 12 = "of Strength" with mid roll
    
    EXPECT_EQ(magicGloves3.getSuffixName(), "of Strength");
    // Mid-range value
    EXPECT_EQ(magicGloves3.getStatBonus(StatType::STRENGTH), 5);
}

// Test for Phase 4, Task 4.3: Item System - Multiple affixes on rare items
TEST_F(ItemTest, RareItemMultipleAffixes) {
    // Create a rare weapon that can have multiple affixes
    Item rareAxe("Battle Axe", ItemType::WEAPON);
    rareAxe.setRarity(ItemRarity::RARE);
    rareAxe.setItemLevel(30);
    
    // Generate affixes - rare items can have multiple prefixes and suffixes
    rareAxe.generateAffixes(100);  // Seed for deterministic generation
    
    // Verify we got multiple affixes
    EXPECT_TRUE(rareAxe.hasPrefix());
    EXPECT_TRUE(rareAxe.hasSuffix());
    
    // Check the generated affixes
    auto affixes = rareAxe.getAffixes();
    EXPECT_GE(affixes.size(), 3);  // Rare items have 3-6 affixes
    EXPECT_LE(affixes.size(), 6);
    
    // Verify specific affixes were applied
    // With seed 100, we expect: "Sharp" prefix, "of Might" suffix, and additional modifiers
    EXPECT_EQ(rareAxe.getFullName(), "Sharp Battle Axe of Might");
    
    // Check damage boost from Sharp prefix
    rareAxe.setDamage(20, 30);
    EXPECT_EQ(rareAxe.getMinDamage(), 24);  // 20 * 1.2
    EXPECT_EQ(rareAxe.getMaxDamage(), 36);  // 30 * 1.2
    
    // Check stat bonuses from multiple affixes
    EXPECT_GT(rareAxe.getStatBonus(StatType::STRENGTH), 0);  // From "of Might"
    EXPECT_GT(rareAxe.getTotalAffixCount(), 2);  // More than just prefix + suffix
}