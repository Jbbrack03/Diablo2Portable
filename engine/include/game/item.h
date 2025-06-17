#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "game/character.h"  // For StatType enum
#include "game/equipment_slot.h"  // For EquipmentSlot enum

namespace d2::game {

enum class ItemType {
    WEAPON,
    ARMOR,
    ACCESSORY,
    CONSUMABLE,
    QUEST,
    GOLD
};

enum class ItemRarity {
    NORMAL,    // Gray - no magical properties
    MAGIC,     // Blue - 1-2 magical properties
    RARE,      // Yellow - 3-6 magical properties
    UNIQUE,    // Gold - fixed unique properties
    SET        // Green - part of a set
};

class Item {
public:
    Item(const std::string& name, ItemType type);
    
    // Factory method
    static std::shared_ptr<Item> createItem(const std::string& name, ItemType type, ItemRarity rarity, int level);
    
    const std::string& getName() const { return m_name; }
    ItemType getType() const { return m_type; }
    int getRequiredLevel() const { return m_requiredLevel; }
    void setRequiredLevel(int level) { m_requiredLevel = level; }
    
    void setItemLevel(int level) { m_itemLevel = level; }
    int getItemLevel() const { return m_itemLevel; }
    
    // Weapon damage
    void setDamage(int minDamage, int maxDamage);
    int getMinDamage() const;
    int getMaxDamage() const;
    
    // Armor defense
    void setDefense(int defense);
    int getDefense() const { return m_defense; }
    
    // Stat bonuses
    void addStatBonus(StatType stat, int bonus);
    int getStatBonus(StatType stat) const;
    
    // Rarity system
    void setRarity(ItemRarity rarity);
    ItemRarity getRarity() const { return m_rarity; }
    int getMaxAffixes() const;
    bool hasFixedStats() const;
    
    // Affix system
    void generatePrefix(int seed);
    bool hasPrefix() const { return !m_prefixName.empty(); }
    const std::string& getPrefixName() const { return m_prefixName; }
    
    void generateSuffix(int seed);
    bool hasSuffix() const { return !m_suffixName.empty(); }
    const std::string& getSuffixName() const { return m_suffixName; }
    
    void generateAffixes(int seed);  // Generate all affixes for rare items
    std::vector<std::string> getAffixes() const;
    int getTotalAffixCount() const;
    
    std::string getFullName() const;
    
    // Item size in inventory grid
    void setSize(int width, int height);
    void setSize(const glm::ivec2& size) { setSize(size.x, size.y); }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    // Equipment slot
    void setEquipmentSlot(EquipmentSlot slot);
    EquipmentSlot getEquipmentSlot() const { return m_equipmentSlot; }
    bool hasEquipmentSlot() const { return m_hasEquipmentSlot; }
    
    // Gold amount (for GOLD type items)
    void setGoldAmount(int amount) { m_goldAmount = amount; }
    int getGoldAmount() const { return m_goldAmount; }
    
    // Quest item properties
    void setQuestId(const std::string& questId) { m_questId = questId; }
    const std::string& getQuestId() const { return m_questId; }
    
    // Two-handed weapon property
    void setTwoHanded(bool twoHanded) { m_isTwoHanded = twoHanded; }
    bool isTwoHanded() const { return m_isTwoHanded; }
    
    // Stat requirements
    void setRequiredStrength(int strength) { m_requiredStrength = strength; }
    int getRequiredStrength() const { return m_requiredStrength; }
    
    void setRequiredDexterity(int dexterity) { m_requiredDexterity = dexterity; }
    int getRequiredDexterity() const { return m_requiredDexterity; }
    
    // Can be equipped in off-hand
    void setCanBeOffHand(bool canBeOffHand) { m_canBeOffHand = canBeOffHand; }
    bool canBeOffHand() const { return m_canBeOffHand; }
    
    // Stackable properties
    void setStackable(bool stackable) { m_isStackable = stackable; }
    bool isStackable() const { return m_isStackable; }
    
    void setMaxStackSize(int maxSize) { m_maxStackSize = maxSize; }
    int getMaxStackSize() const { return m_maxStackSize; }
    
    void setQuantity(int quantity) { m_quantity = quantity; }
    int getQuantity() const { return m_quantity; }
    
    bool canStackWith(const Item& other) const;
    
private:
    std::string m_name;
    ItemType m_type;
    int m_requiredLevel = 1;
    int m_itemLevel = 1;  // Item level for affix generation
    
    // Combat stats
    int m_minDamage = 0;
    int m_maxDamage = 0;
    int m_defense = 0;
    
    // Stat bonuses
    std::unordered_map<StatType, int> m_statBonuses;
    
    // Rarity
    ItemRarity m_rarity = ItemRarity::NORMAL;
    
    // Affix data
    std::string m_prefixName;
    std::string m_suffixName;
    std::vector<std::string> m_additionalAffixes;  // For rare items with multiple affixes
    
    // Size in inventory grid
    int m_width = 1;
    int m_height = 1;
    
    // Equipment slot
    EquipmentSlot m_equipmentSlot;
    bool m_hasEquipmentSlot = false;
    
    // Gold amount
    int m_goldAmount = 0;
    
    // Quest item data
    std::string m_questId;
    
    // Weapon properties
    bool m_isTwoHanded = false;
    bool m_canBeOffHand = false;
    
    // Stat requirements
    int m_requiredStrength = 0;
    int m_requiredDexterity = 0;
    
    // Stacking properties
    bool m_isStackable = false;
    int m_maxStackSize = 1;
    int m_quantity = 1;
};

} // namespace d2::game