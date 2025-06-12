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