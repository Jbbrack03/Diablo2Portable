#include "game/item.h"
#include <algorithm>
#include <random>

namespace d2::game {

Item::Item(const std::string& name, ItemType type)
    : m_name(name), m_type(type) {
}

void Item::setDamage(int minDamage, int maxDamage) {
    m_minDamage = minDamage;
    m_maxDamage = maxDamage;
}

int Item::getMinDamage() const {
    return m_minDamage + getStatBonus(StatType::DAMAGE);
}

int Item::getMaxDamage() const {
    return m_maxDamage + getStatBonus(StatType::DAMAGE);
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
        case ItemRarity::NORMAL: return 0;
        case ItemRarity::MAGIC: return 2;  // 1 prefix + 1 suffix
        case ItemRarity::RARE: return 6;   // up to 3 prefix + 3 suffix
        case ItemRarity::UNIQUE: return 0; // fixed stats
        case ItemRarity::SET: return 0;    // fixed stats
    }
    return 0;
}

bool Item::hasFixedStats() const {
    return m_rarity == ItemRarity::UNIQUE || m_rarity == ItemRarity::SET;
}

void Item::generatePrefix(int seed) {
    if (m_rarity == ItemRarity::NORMAL || hasFixedStats()) return;
    
    static std::mt19937 gen(seed);
    std::uniform_int_distribution<> prefixDist(0, 2);
    
    int prefixType = prefixDist(gen);
    switch (prefixType) {
        case 0:  // Damage prefix
            m_prefixName = "Sharp";
            m_minDamage = m_minDamage * 1.2;
            m_maxDamage = m_maxDamage * 1.2;
            break;
        case 1:  // Defense prefix
            m_prefixName = "Sturdy";
            m_defense = m_defense * 1.3;
            break;
        case 2:  // Enhanced damage
            m_prefixName = "Cruel";
            m_minDamage = m_minDamage * 1.5;
            m_maxDamage = m_maxDamage * 1.5;
            break;
    }
}

void Item::generateSuffix(int seed) {
    if (m_rarity == ItemRarity::NORMAL || hasFixedStats()) return;
    
    static std::mt19937 gen(seed);
    std::uniform_int_distribution<> suffixDist(0, 3);
    std::uniform_int_distribution<> valueDist(1, 10);
    
    int suffixType = suffixDist(gen);
    switch (suffixType) {
        case 0:  // Strength suffix
            m_suffixName = "of the Bear";
            addStatBonus(StatType::STRENGTH, valueDist(gen));
            break;
        case 1:  // Dexterity suffix
            m_suffixName = "of the Fox";
            addStatBonus(StatType::DEXTERITY, valueDist(gen));
            break;
        case 2:  // Vitality suffix
            m_suffixName = "of the Wolf";
            addStatBonus(StatType::VITALITY, valueDist(gen));
            break;
        case 3:  // Energy suffix
            m_suffixName = "of the Eagle";
            addStatBonus(StatType::ENERGY, valueDist(gen));
            break;
    }
}

void Item::generateAffixes(int seed) {
    if (m_rarity == ItemRarity::NORMAL || hasFixedStats()) return;
    
    static std::mt19937 gen(seed);
    
    if (m_rarity == ItemRarity::MAGIC) {
        // Generate one prefix and/or one suffix
        std::uniform_int_distribution<> affixDist(0, 2);
        int affixChoice = affixDist(gen);
        
        if (affixChoice == 0 || affixChoice == 2) {
            generatePrefix(seed);
        }
        if (affixChoice == 1 || affixChoice == 2) {
            generateSuffix(seed + 1);
        }
    } else if (m_rarity == ItemRarity::RARE) {
        // Generate 3-6 affixes
        std::uniform_int_distribution<> numAffixDist(3, 6);
        int numAffixes = numAffixDist(gen);
        
        // Always generate at least one prefix and one suffix
        generatePrefix(seed);
        generateSuffix(seed + 1);
        
        // Add additional affixes
        for (int i = 2; i < numAffixes; i++) {
            std::string affixName = "Enhanced Property " + std::to_string(i - 1);
            m_additionalAffixes.push_back(affixName);
            
            // Add random stat bonus
            std::uniform_int_distribution<> statDist(0, 3);
            std::uniform_int_distribution<> valueDist(5, 15);
            StatType stat = static_cast<StatType>(statDist(gen));
            addStatBonus(stat, valueDist(gen));
        }
    }
}

std::vector<std::string> Item::getAffixes() const {
    std::vector<std::string> affixes;
    
    if (!m_prefixName.empty()) {
        affixes.push_back(m_prefixName);
    }
    
    if (!m_suffixName.empty()) {
        affixes.push_back(m_suffixName);
    }
    
    affixes.insert(affixes.end(), m_additionalAffixes.begin(), m_additionalAffixes.end());
    
    return affixes;
}

int Item::getTotalAffixCount() const {
    int count = 0;
    if (!m_prefixName.empty()) count++;
    if (!m_suffixName.empty()) count++;
    count += m_additionalAffixes.size();
    return count;
}

std::string Item::getFullName() const {
    std::string fullName;
    
    if (!m_prefixName.empty()) {
        fullName += m_prefixName + " ";
    }
    
    fullName += m_name;
    
    if (!m_suffixName.empty()) {
        fullName += " " + m_suffixName;
    }
    
    return fullName;
}

void Item::setSize(int width, int height) {
    m_width = width;
    m_height = height;
}

void Item::setEquipmentSlot(EquipmentSlot slot) {
    m_equipmentSlot = slot;
    m_hasEquipmentSlot = true;
}

bool Item::canStackWith(const Item& other) const {
    // Can't stack if either item is not stackable
    if (!m_isStackable || !other.isStackable()) {
        return false;
    }
    
    // Must be same item type and name
    if (m_type != other.getType() || m_name != other.getName()) {
        return false;
    }
    
    // Can't stack items with different properties
    if (m_rarity != ItemRarity::NORMAL || other.getRarity() != ItemRarity::NORMAL) {
        return false;  // Only stack normal items
    }
    
    return true;
}

} // namespace d2::game