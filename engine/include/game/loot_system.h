#pragma once

#include <memory>
#include <vector>

namespace d2::game {

// Forward declarations
class Item;
class Monster;

class LootSystem {
public:
    LootSystem();
    
    // Generate loot drops for a defeated monster
    std::vector<std::shared_ptr<Item>> generateLoot(std::shared_ptr<Monster> monster);
    
private:
    // Helper methods
    std::shared_ptr<Item> generateRandomItem(int monsterLevel);
};

} // namespace d2::game