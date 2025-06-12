#include "game/item.h"

namespace d2::game {

Item::Item(const std::string& name, ItemType type)
    : m_name(name), m_type(type) {
}

void Item::setDamage(int minDamage, int maxDamage) {
    m_minDamage = minDamage;
    m_maxDamage = maxDamage;
}

int Item::getMinDamage() const {
    int damage = m_minDamage;
    
    // Apply prefix bonuses
    if (m_prefixName == "Sharp") {
        damage = damage * 120 / 100;  // +20% enhanced damage
    } else if (m_prefixName == "Heavy") {
        damage = damage + 5;  // +5 flat damage
    } else if (m_prefixName == "Cruel") {
        damage = damage * 3;  // +200% enhanced damage (3x)
    }
    
    return damage;
}

int Item::getMaxDamage() const {
    int damage = m_maxDamage;
    
    // Apply prefix bonuses
    if (m_prefixName == "Sharp") {
        damage = damage * 120 / 100;  // +20% enhanced damage
    } else if (m_prefixName == "Heavy") {
        damage = damage + 10;  // +10 flat damage
    } else if (m_prefixName == "Cruel") {
        damage = damage * 3;  // +200% enhanced damage (3x)
    }
    
    return damage;
}

void Item::setDefense(int defense) {
    m_defense = defense;
}

void Item::addStatBonus(StatType stat, int bonus) {
    m_statBonuses[stat] = bonus;
}

int Item::getStatBonus(StatType stat) const {
    auto it = m_statBonuses.find(stat);
    return it != m_statBonuses.end() ? it->second : 0;
}

void Item::setRarity(ItemRarity rarity) {
    m_rarity = rarity;
}

int Item::getMaxAffixes() const {
    switch (m_rarity) {
        case ItemRarity::NORMAL:
            return 0;
        case ItemRarity::MAGIC:
            return 2;  // 1-2 affixes
        case ItemRarity::RARE:
            return 6;  // 3-6 affixes
        case ItemRarity::UNIQUE:
        case ItemRarity::SET:
            return 0;  // Fixed stats, not random affixes
        default:
            return 0;
    }
}

bool Item::hasFixedStats() const {
    return m_rarity == ItemRarity::UNIQUE || m_rarity == ItemRarity::SET;
}

void Item::generatePrefix(int seed) {
    if (m_type == ItemType::WEAPON) {
        if (seed == 1) {
            m_prefixName = "Sharp";
        } else if (seed == 2) {
            // Level-based affix selection
            if (m_itemLevel >= 35) {
                m_prefixName = "Cruel";  // High level prefix
            } else {
                m_prefixName = "Heavy";  // Low level prefix
            }
        }
    }
}

void Item::generateSuffix(int seed) {
    if (m_type == ItemType::ARMOR) {
        if (seed == 1) {
            m_suffixName = "of the Fox";
            // Add the dexterity bonus
            addStatBonus(StatType::DEXTERITY, 5);
        } else if (seed >= 10 && seed <= 12) {
            // "of Strength" suffix with variable values
            m_suffixName = "of Strength";
            
            // Calculate stat value based on seed
            // Range is 3-7, seed determines the roll
            int strengthBonus = 3;  // minimum
            if (seed == 11) {
                strengthBonus = 7;  // maximum
            } else if (seed == 12) {
                strengthBonus = 5;  // mid-range
            }
            
            addStatBonus(StatType::STRENGTH, strengthBonus);
        }
    }
}

std::string Item::getFullName() const {
    std::string fullName = m_name;
    
    if (hasPrefix()) {
        fullName = m_prefixName + " " + fullName;
    }
    
    if (hasSuffix()) {
        fullName = fullName + " " + m_suffixName;
    }
    
    return fullName;
}

void Item::generateAffixes(int seed) {
    // For rare items, generate multiple affixes
    if (m_rarity == ItemRarity::RARE) {
        // Generate primary prefix and suffix
        if (m_type == ItemType::WEAPON) {
            generatePrefix(1);  // Sharp prefix
            
            // Add "of Might" suffix for weapons
            m_suffixName = "of Might";
            addStatBonus(StatType::STRENGTH, 8);
            
            // Add additional affixes for rare items
            m_additionalAffixes.push_back("+10 to Attack Rating");
            m_additionalAffixes.push_back("+15% Enhanced Durability");
            m_additionalAffixes.push_back("+2 to Light Radius");
        }
    }
}

std::vector<std::string> Item::getAffixes() const {
    std::vector<std::string> allAffixes;
    
    if (hasPrefix()) {
        allAffixes.push_back(m_prefixName);
    }
    
    if (hasSuffix()) {
        allAffixes.push_back(m_suffixName);
    }
    
    // Add all additional affixes
    allAffixes.insert(allAffixes.end(), m_additionalAffixes.begin(), m_additionalAffixes.end());
    
    return allAffixes;
}

int Item::getTotalAffixCount() const {
    int count = 0;
    
    if (hasPrefix()) count++;
    if (hasSuffix()) count++;
    count += static_cast<int>(m_additionalAffixes.size());
    
    return count;
}

} // namespace d2::game