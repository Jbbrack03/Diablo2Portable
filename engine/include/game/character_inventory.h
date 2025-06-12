#pragma once

#include <memory>
#include <unordered_map>
#include "game/inventory.h"
#include "game/equipment_slot.h"

namespace d2::game {

// Forward declaration
class Item;

class CharacterInventory {
public:
    CharacterInventory();
    
    // Equipment management
    bool equipItem(std::shared_ptr<Item> item);
    bool unequipItem(EquipmentSlot slot);
    std::shared_ptr<Item> getEquippedItem(EquipmentSlot slot) const;
    
    // Access to backpack inventory
    Inventory& getBackpack() { return m_backpack; }
    const Inventory& getBackpack() const { return m_backpack; }
    
private:
    Inventory m_backpack;  // 10x4 standard inventory
    std::unordered_map<EquipmentSlot, std::shared_ptr<Item>> m_equipped;
};

} // namespace d2::game