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

// Gold drop range
struct GoldRange {
    int minLevel;
    int maxLevel;
    int minGold;
    int maxGold;
};

// Quest item information
struct QuestItemInfo {
    std::string name;
    std::string questId;
    float dropChance;
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
    
    // Configure gold drops
    void setGoldDropChance(float chance) { m_goldDropChance = chance; }
    void setGoldRange(int minLevel, int maxLevel, int minGold, int maxGold);
    
    // Configure quest drops
    void addQuestDrop(MonsterType monsterType, const QuestItemInfo& questItem);
    
private:
    // Helper methods
    std::shared_ptr<Item> generateRandomItem(int monsterLevel);
    ItemType selectItemType(MonsterType monsterType);
    ItemRarity selectRarity(int monsterLevel);
    std::shared_ptr<Item> generateGold(int monsterLevel);
    int selectGoldAmount(int monsterLevel);
    
    // Monster-specific loot tables
    std::unordered_map<MonsterType, std::vector<LootTableEntry>> m_monsterLootTables;
    
    // Rarity chances by level range
    std::vector<RarityLevel> m_rarityLevels;
    
    // Gold drop configuration
    float m_goldDropChance = 0.5f;  // Default 50% chance
    std::vector<GoldRange> m_goldRanges;
    
    // Quest drop configuration
    std::unordered_map<MonsterType, std::vector<QuestItemInfo>> m_questDrops;
};

} // namespace d2::game