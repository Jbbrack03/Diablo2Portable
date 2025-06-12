#pragma once

#include <string>
#include <unordered_map>
#include <vector>
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
};

} // namespace d2::game