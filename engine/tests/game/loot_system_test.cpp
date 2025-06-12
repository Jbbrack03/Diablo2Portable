#include <gtest/gtest.h>
#include <memory>
#include <map>
#include "game/loot_system.h"
#include "game/item.h"
#include "game/monster.h"

using namespace d2::game;

class LootSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup will go here
    }
};

// Test for Phase 4, Task 4.6: Loot System - Basic loot generation
TEST_F(LootSystemTest, BasicLootGeneration) {
    LootSystem lootSystem;
    
    // Create a monster
    auto zombie = std::make_shared<Monster>(MonsterType::ZOMBIE, 5);
    
    // Generate loot for the monster
    auto lootDrops = lootSystem.generateLoot(zombie);
    
    // Should generate at least some loot
    EXPECT_FALSE(lootDrops.empty());
    
    // Check that generated items have appropriate properties
    for (const auto& item : lootDrops) {
        EXPECT_FALSE(item->getName().empty());
        EXPECT_LE(item->getRequiredLevel(), zombie->getLevel() + 2); // Items shouldn't require much higher level
    }
    
    // Test with higher level monster
    auto demon = std::make_shared<Monster>(MonsterType::DEMON, 50);
    auto demonLoot = lootSystem.generateLoot(demon);
    
    // Higher level monsters should have potential for better loot
    bool hasHighLevelItem = false;
    for (const auto& item : demonLoot) {
        if (item->getItemLevel() >= 45) {
            hasHighLevelItem = true;
            break;
        }
    }
    EXPECT_TRUE(hasHighLevelItem);
}

// Test for Phase 4, Task 4.6: Loot System - Monster-specific loot tables
TEST_F(LootSystemTest, MonsterSpecificLootTables) {
    LootSystem lootSystem;
    
    // Configure loot tables for specific monsters
    // Skeletons drop more weapons
    std::vector<LootTableEntry> skeletonTable = {
        {ItemType::WEAPON, 0.7f},    // 70% chance for weapons
        {ItemType::ARMOR, 0.2f},     // 20% chance for armor
        {ItemType::CONSUMABLE, 0.1f} // 10% chance for consumables
    };
    lootSystem.setMonsterLootTable(MonsterType::SKELETON, skeletonTable);
    
    // Zombies drop more consumables
    std::vector<LootTableEntry> zombieTable = {
        {ItemType::WEAPON, 0.2f},
        {ItemType::ARMOR, 0.2f},
        {ItemType::CONSUMABLE, 0.6f}  // 60% chance for consumables
    };
    lootSystem.setMonsterLootTable(MonsterType::ZOMBIE, zombieTable);
    
    // Test skeleton drops - should favor weapons
    int weaponCount = 0;
    int totalDrops = 0;
    
    for (int i = 0; i < 100; i++) {
        auto skeleton = std::make_shared<Monster>(MonsterType::SKELETON, 10);
        auto loot = lootSystem.generateLoot(skeleton);
        
        for (const auto& item : loot) {
            totalDrops++;
            if (item->getType() == ItemType::WEAPON) {
                weaponCount++;
            }
        }
    }
    
    // With 70% weapon chance, expect at least 60% weapons in practice
    float weaponRatio = static_cast<float>(weaponCount) / totalDrops;
    EXPECT_GT(weaponRatio, 0.6f);
    
    // Test zombie drops - should favor consumables
    int consumableCount = 0;
    totalDrops = 0;
    
    for (int i = 0; i < 100; i++) {
        auto zombie = std::make_shared<Monster>(MonsterType::ZOMBIE, 10);
        auto loot = lootSystem.generateLoot(zombie);
        
        for (const auto& item : loot) {
            totalDrops++;
            if (item->getType() == ItemType::CONSUMABLE) {
                consumableCount++;
            }
        }
    }
    
    // With 60% consumable chance, expect at least 50% consumables in practice
    float consumableRatio = static_cast<float>(consumableCount) / totalDrops;
    EXPECT_GT(consumableRatio, 0.5f);
}

// Test for Phase 4 Enhancement: Gold drops
TEST_F(LootSystemTest, GoldDrops) {
    LootSystem lootSystem;
    
    // Configure gold drop chances
    lootSystem.setGoldDropChance(0.8f);  // 80% chance to drop gold
    
    // Configure gold amount ranges based on monster level
    lootSystem.setGoldRange(1, 10, 5, 50);      // Level 1-10: 5-50 gold
    lootSystem.setGoldRange(11, 30, 50, 250);   // Level 11-30: 50-250 gold
    lootSystem.setGoldRange(31, 50, 200, 1000); // Level 31-50: 200-1000 gold
    
    // Test low level monster gold drops
    int goldDropCount = 0;
    int totalGoldAmount = 0;
    const int numTests = 100;
    
    for (int i = 0; i < numTests; i++) {
        auto zombie = std::make_shared<Monster>(MonsterType::ZOMBIE, 5);
        auto loot = lootSystem.generateLoot(zombie);
        
        // Check for gold in loot
        for (const auto& item : loot) {
            if (item->getType() == ItemType::GOLD) {
                goldDropCount++;
                totalGoldAmount += item->getGoldAmount();
                
                // Verify gold is in correct range for level 5 monster
                EXPECT_GE(item->getGoldAmount(), 5);
                EXPECT_LE(item->getGoldAmount(), 50);
            }
        }
    }
    
    // With 80% drop chance, expect at least 70% drops
    float dropRate = static_cast<float>(goldDropCount) / numTests;
    EXPECT_GT(dropRate, 0.7f);
    
    // Average gold should be reasonable for level range
    float avgGold = static_cast<float>(totalGoldAmount) / goldDropCount;
    EXPECT_GT(avgGold, 10.0f);  // Should average more than minimum
    EXPECT_LT(avgGold, 40.0f);  // Should average less than maximum
    
    // Test high level monster gold drops
    goldDropCount = 0;
    totalGoldAmount = 0;
    
    for (int i = 0; i < numTests; i++) {
        auto demon = std::make_shared<Monster>(MonsterType::DEMON, 40);
        auto loot = lootSystem.generateLoot(demon);
        
        for (const auto& item : loot) {
            if (item->getType() == ItemType::GOLD) {
                goldDropCount++;
                totalGoldAmount += item->getGoldAmount();
                
                // Verify gold is in correct range for level 40 monster
                EXPECT_GE(item->getGoldAmount(), 200);
                EXPECT_LE(item->getGoldAmount(), 1000);
            }
        }
    }
    
    // Higher level monsters should drop more gold on average
    float highLevelAvgGold = static_cast<float>(totalGoldAmount) / goldDropCount;
    EXPECT_GT(highLevelAvgGold, avgGold * 10);  // Significantly more gold
}

// Test for Phase 4, Task 4.6: Loot System - Rarity chances
TEST_F(LootSystemTest, LootRarityChances) {
    LootSystem lootSystem;
    
    // Configure rarity chances based on monster level
    // Low level monsters - mostly normal items
    std::vector<RarityChance> lowLevelChances = {
        {ItemRarity::NORMAL, 0.7f},   // 70% normal
        {ItemRarity::MAGIC, 0.25f},   // 25% magic
        {ItemRarity::RARE, 0.05f},    // 5% rare
        {ItemRarity::UNIQUE, 0.0f},   // 0% unique
        {ItemRarity::SET, 0.0f}       // 0% set
    };
    lootSystem.setRarityChances(1, 10, lowLevelChances);
    
    // High level monsters - better rarity chances
    std::vector<RarityChance> highLevelChances = {
        {ItemRarity::NORMAL, 0.3f},   // 30% normal
        {ItemRarity::MAGIC, 0.4f},    // 40% magic
        {ItemRarity::RARE, 0.2f},     // 20% rare
        {ItemRarity::UNIQUE, 0.07f},  // 7% unique
        {ItemRarity::SET, 0.03f}      // 3% set
    };
    lootSystem.setRarityChances(40, 100, highLevelChances);
    
    // Test low level monster drops
    std::map<ItemRarity, int> lowLevelRarities;
    int totalLowLevel = 0;
    
    for (int i = 0; i < 200; i++) {
        auto zombie = std::make_shared<Monster>(MonsterType::ZOMBIE, 5);
        auto loot = lootSystem.generateLoot(zombie);
        
        for (const auto& item : loot) {
            lowLevelRarities[item->getRarity()]++;
            totalLowLevel++;
        }
    }
    
    // Check that most are normal items
    float normalRatio = static_cast<float>(lowLevelRarities[ItemRarity::NORMAL]) / totalLowLevel;
    EXPECT_GT(normalRatio, 0.6f);  // At least 60% normal
    
    // Should have very few or no unique/set items
    EXPECT_EQ(lowLevelRarities[ItemRarity::UNIQUE], 0);
    EXPECT_EQ(lowLevelRarities[ItemRarity::SET], 0);
    
    // Test high level monster drops
    std::map<ItemRarity, int> highLevelRarities;
    int totalHighLevel = 0;
    
    for (int i = 0; i < 200; i++) {
        auto demon = std::make_shared<Monster>(MonsterType::DEMON, 50);
        auto loot = lootSystem.generateLoot(demon);
        
        for (const auto& item : loot) {
            highLevelRarities[item->getRarity()]++;
            totalHighLevel++;
        }
    }
    
    // Should have more magic and rare items
    float magicRareRatio = static_cast<float>(highLevelRarities[ItemRarity::MAGIC] + 
                                               highLevelRarities[ItemRarity::RARE]) / totalHighLevel;
    EXPECT_GT(magicRareRatio, 0.5f);  // At least 50% magic or rare
    
    // Should have some unique/set items
    int uniqueSetCount = highLevelRarities[ItemRarity::UNIQUE] + highLevelRarities[ItemRarity::SET];
    EXPECT_GT(uniqueSetCount, 0);  // At least one unique or set item
}