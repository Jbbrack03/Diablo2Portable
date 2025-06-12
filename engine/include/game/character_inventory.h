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
    
    // Equipment validation
    bool equipItemWithValidation(std::shared_ptr<Item> item);
    bool forceEquipToSlot(std::shared_ptr<Item> item, EquipmentSlot slot);
    bool isTwoHandedEquipped() const;
    
    // Character stats for requirements
    void setCharacterLevel(int level) { m_characterLevel = level; }
    void setCharacterStrength(int strength) { m_characterStrength = strength; }
    void setCharacterDexterity(int dexterity) { m_characterDexterity = dexterity; }
    
    int getCharacterLevel() const { return m_characterLevel; }
    int getCharacterStrength() const { return m_characterStrength; }
    int getCharacterDexterity() const { return m_characterDexterity; }
    
private:
    Inventory m_backpack;  // 10x4 standard inventory
    std::unordered_map<EquipmentSlot, std::shared_ptr<Item>> m_equipped;
    
    // Character stats
    int m_characterLevel = 1;
    int m_characterStrength = 10;  // Base strength
    int m_characterDexterity = 10; // Base dexterity
};

} // namespace d2::game