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

bool CharacterInventory::equipItemWithValidation(std::shared_ptr<Item> item) {
    if (!item || !item->hasEquipmentSlot()) {
        return false;
    }
    
    // Check level requirement
    if (item->getRequiredLevel() > m_characterLevel) {
        return false;
    }
    
    // Check stat requirements
    if (item->getRequiredStrength() > m_characterStrength) {
        return false;
    }
    
    if (item->getRequiredDexterity() > m_characterDexterity) {
        return false;
    }
    
    EquipmentSlot slot = item->getEquipmentSlot();
    
    // Check if trying to equip in off-hand while two-handed weapon is equipped
    if (slot == EquipmentSlot::OFF_HAND && isTwoHandedEquipped()) {
        return false;
    }
    
    // Check if trying to equip two-handed weapon while off-hand is occupied
    if (item->isTwoHanded() && getEquippedItem(EquipmentSlot::OFF_HAND) != nullptr) {
        return false;
    }
    
    // Use regular equip method if all validations pass
    return equipItem(item);
}

bool CharacterInventory::forceEquipToSlot(std::shared_ptr<Item> item, EquipmentSlot slot) {
    if (!item || !item->hasEquipmentSlot()) {
        return false;
    }
    
    // Check if item's equipment slot matches the requested slot
    if (item->getEquipmentSlot() != slot) {
        return false;  // Can't equip item to wrong slot
    }
    
    // Use regular equip method
    return equipItem(item);
}

bool CharacterInventory::isTwoHandedEquipped() const {
    auto mainHand = getEquippedItem(EquipmentSlot::MAIN_HAND);
    return mainHand && mainHand->isTwoHanded();
}

} // namespace d2::game