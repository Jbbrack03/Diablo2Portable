#pragma once

#include <string>
#include <unordered_map>
#include "game/character.h"  // For StatType enum

namespace d2::game {

enum class ItemType {
    WEAPON,
    ARMOR,
    ACCESSORY,
    CONSUMABLE,
    QUEST
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
    
    const std::string& getName() const { return m_name; }
    ItemType getType() const { return m_type; }
    int getRequiredLevel() const { return m_requiredLevel; }
    
    // Weapon damage
    void setDamage(int minDamage, int maxDamage);
    int getMinDamage() const { return m_minDamage; }
    int getMaxDamage() const { return m_maxDamage; }
    
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
    
private:
    std::string m_name;
    ItemType m_type;
    int m_requiredLevel = 1;
    
    // Combat stats
    int m_minDamage = 0;
    int m_maxDamage = 0;
    int m_defense = 0;
    
    // Stat bonuses
    std::unordered_map<StatType, int> m_statBonuses;
    
    // Rarity
    ItemRarity m_rarity = ItemRarity::NORMAL;
};

} // namespace d2::game