#include "game/loot_system.h"
#include <random>
#include <algorithm>

namespace d2::game {

LootSystem::LootSystem() {
}

std::vector<std::shared_ptr<Item>> LootSystem::generateLoot(std::shared_ptr<Monster> monster) {
    std::vector<std::shared_ptr<Item>> loot;
    
    if (!monster) {
        return loot;
    }
    
    int monsterLevel = monster->getLevel();
    
    // Simple loot generation - number of items based on monster level
    int numItems = 1;
    if (monsterLevel >= 10) numItems = 2;
    if (monsterLevel >= 30) numItems = 3;
    
    for (int i = 0; i < numItems; i++) {
        auto item = generateRandomItem(monsterLevel);
        if (item) {
            // Update item type based on monster loot table if available
            auto tableIt = m_monsterLootTables.find(monster->getType());
            if (tableIt != m_monsterLootTables.end()) {
                ItemType selectedType = selectItemType(monster->getType());
                
                // Recreate item with selected type
                switch (selectedType) {
                    case ItemType::WEAPON:
                        item = std::make_shared<Item>("Sword", ItemType::WEAPON);
                        item->setDamage(monsterLevel, monsterLevel * 2);
                        break;
                    case ItemType::ARMOR:
                        item = std::make_shared<Item>("Armor", ItemType::ARMOR);
                        item->setDefense(monsterLevel * 3);
                        break;
                    case ItemType::CONSUMABLE:
                        item = std::make_shared<Item>("Potion", ItemType::CONSUMABLE);
                        break;
                    default:
                        break;
                }
                
                // Re-set item properties
                item->setItemLevel(std::max(1, monsterLevel - 5 + (rand() % 10)));
                item->setRequiredLevel(std::max(1, monsterLevel - 2));
            }
            
            // Set rarity based on monster level
            ItemRarity rarity = selectRarity(monsterLevel);
            item->setRarity(rarity);
            
            // Generate affixes for magic/rare items
            if (rarity == ItemRarity::MAGIC || rarity == ItemRarity::RARE) {
                item->generateAffixes(rand());
            }
            
            loot.push_back(item);
        }
    }
    
    return loot;
}

std::shared_ptr<Item> LootSystem::generateRandomItem(int monsterLevel) {
    // Simple random item generation
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> typeDist(0, 2);
    
    std::shared_ptr<Item> item;
    int itemType = typeDist(gen);
    
    switch (itemType) {
        case 0: // Weapon
            item = std::make_shared<Item>("Sword", ItemType::WEAPON);
            item->setDamage(monsterLevel, monsterLevel * 2);
            break;
        case 1: // Armor
            item = std::make_shared<Item>("Armor", ItemType::ARMOR);
            item->setDefense(monsterLevel * 3);
            break;
        case 2: // Consumable
            item = std::make_shared<Item>("Potion", ItemType::CONSUMABLE);
            break;
    }
    
    if (item) {
        // Set item level based on monster level
        item->setItemLevel(std::max(1, monsterLevel - 5 + (typeDist(gen) * 3)));
        
        // Set required level
        int reqLevel = std::max(1, monsterLevel - 2);
        item->setRequiredLevel(reqLevel);
        
        // Set default rarity if not set elsewhere
        if (!item) return item;
        item->setRarity(ItemRarity::NORMAL);
    }
    
    return item;
}

void LootSystem::setMonsterLootTable(MonsterType type, const std::vector<LootTableEntry>& table) {
    m_monsterLootTables[type] = table;
}

ItemType LootSystem::selectItemType(MonsterType monsterType) {
    auto tableIt = m_monsterLootTables.find(monsterType);
    if (tableIt == m_monsterLootTables.end()) {
        // Default random selection
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 2);
        return static_cast<ItemType>(dist(gen));
    }
    
    // Use weighted random selection based on loot table
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);
    
    float roll = dist(gen);
    float cumulative = 0.0f;
    
    for (const auto& entry : tableIt->second) {
        cumulative += entry.chance;
        if (roll <= cumulative) {
            return entry.type;
        }
    }
    
    // Fallback to first entry if probabilities don't sum to 1.0
    return tableIt->second.front().type;
}

void LootSystem::setRarityChances(int minLevel, int maxLevel, const std::vector<RarityChance>& chances) {
    RarityLevel level;
    level.minLevel = minLevel;
    level.maxLevel = maxLevel;
    level.chances = chances;
    
    // Remove any existing level range that overlaps
    m_rarityLevels.erase(
        std::remove_if(m_rarityLevels.begin(), m_rarityLevels.end(),
            [minLevel, maxLevel](const RarityLevel& rl) {
                return (rl.minLevel <= maxLevel && rl.maxLevel >= minLevel);
            }),
        m_rarityLevels.end()
    );
    
    m_rarityLevels.push_back(level);
}

ItemRarity LootSystem::selectRarity(int monsterLevel) {
    // Find the appropriate rarity level range
    for (const auto& rarityLevel : m_rarityLevels) {
        if (monsterLevel >= rarityLevel.minLevel && monsterLevel <= rarityLevel.maxLevel) {
            // Use weighted random selection
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<> dist(0.0, 1.0);
            
            float roll = dist(gen);
            float cumulative = 0.0f;
            
            for (const auto& chance : rarityLevel.chances) {
                cumulative += chance.chance;
                if (roll <= cumulative) {
                    return chance.rarity;
                }
            }
        }
    }
    
    // Default to normal if no range matches
    return ItemRarity::NORMAL;
}

} // namespace d2::game