#include "game/character_inventory.h"
#include "game/item.h"

namespace d2::game {

CharacterInventory::CharacterInventory()
    : m_backpack(10, 4) {  // Standard Diablo II inventory size
}

bool CharacterInventory::equipItem(std::shared_ptr<Item> item) {
    if (!item || !item->hasEquipmentSlot()) {
        return false;
    }
    
    EquipmentSlot slot = item->getEquipmentSlot();
    
    // If slot is already occupied, move current item to backpack
    auto currentItem = m_equipped[slot];
    if (currentItem) {
        // Try to find space in backpack
        bool added = false;
        for (int y = 0; y < m_backpack.getHeight() && !added; y++) {
            for (int x = 0; x < m_backpack.getWidth() && !added; x++) {
                if (m_backpack.addItem(currentItem, x, y)) {
                    added = true;
                }
            }
        }
        
        if (!added) {
            return false;  // No space in backpack for swap
        }
    }
    
    // Equip the new item
    m_equipped[slot] = item;
    return true;
}

bool CharacterInventory::unequipItem(EquipmentSlot slot) {
    auto item = m_equipped[slot];
    if (!item) {
        return false;  // Nothing equipped in this slot
    }
    
    // Try to add to backpack
    bool added = false;
    for (int y = 0; y < m_backpack.getHeight() && !added; y++) {
        for (int x = 0; x < m_backpack.getWidth() && !added; x++) {
            if (m_backpack.addItem(item, x, y)) {
                added = true;
            }
        }
    }
    
    if (!added) {
        return false;  // No space in backpack
    }
    
    // Remove from equipped
    m_equipped.erase(slot);
    return true;
}

std::shared_ptr<Item> CharacterInventory::getEquippedItem(EquipmentSlot slot) const {
    auto it = m_equipped.find(slot);
    return it != m_equipped.end() ? it->second : nullptr;
}

} // namespace d2::game