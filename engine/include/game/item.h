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
};

} // namespace d2::game