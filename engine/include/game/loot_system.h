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

class LootSystem {
public:
    LootSystem();
    
    // Generate loot drops for a defeated monster
    std::vector<std::shared_ptr<Item>> generateLoot(std::shared_ptr<Monster> monster);
    
    // Configure loot tables
    void setMonsterLootTable(MonsterType type, const std::vector<LootTableEntry>& table);
    
private:
    // Helper methods
    std::shared_ptr<Item> generateRandomItem(int monsterLevel);
    ItemType selectItemType(MonsterType monsterType);
    
    // Monster-specific loot tables
    std::unordered_map<MonsterType, std::vector<LootTableEntry>> m_monsterLootTables;
};

} // namespace d2::game