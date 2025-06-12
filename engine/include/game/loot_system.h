#pragma once

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include "game/item.h"      // For ItemType
#include "game/monster.h"   // For MonsterType

namespace d2::game {

// Forward declarations
class Item;
class Monster;

// Loot table entry
struct LootTableEntry {
    ItemType type;
    float chance;
};

// Rarity chance entry
struct RarityChance {
    ItemRarity rarity;
    float chance;
};

// Level range for rarity chances
struct RarityLevel {
    int minLevel;
    int maxLevel;
    std::vector<RarityChance> chances;
};

class LootSystem {
public:
    LootSystem();
    
    // Generate loot drops for a defeated monster
    std::vector<std::shared_ptr<Item>> generateLoot(std::shared_ptr<Monster> monster);
    
    // Configure loot tables
    void setMonsterLootTable(MonsterType type, const std::vector<LootTableEntry>& table);
    
    // Configure rarity chances
    void setRarityChances(int minLevel, int maxLevel, const std::vector<RarityChance>& chances);
    
private:
    // Helper methods
    std::shared_ptr<Item> generateRandomItem(int monsterLevel);
    ItemType selectItemType(MonsterType monsterType);
    ItemRarity selectRarity(int monsterLevel);
    
    // Monster-specific loot tables
    std::unordered_map<MonsterType, std::vector<LootTableEntry>> m_monsterLootTables;
    
    // Rarity chances by level range
    std::vector<RarityLevel> m_rarityLevels;
};

} // namespace d2::game