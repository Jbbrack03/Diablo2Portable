#include "game/loot_system.h"
#include "game/item.h"
#include "game/monster.h"
#include <random>

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
    }
    
    return item;
}

} // namespace d2::game