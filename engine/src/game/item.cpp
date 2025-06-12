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
    // Sharp prefix adds 20% enhanced damage
    if (m_prefixName == "Sharp") {
        damage = damage * 120 / 100;  // +20%
    }
    return damage;
}

int Item::getMaxDamage() const {
    int damage = m_maxDamage;
    // Sharp prefix adds 20% enhanced damage
    if (m_prefixName == "Sharp") {
        damage = damage * 120 / 100;  // +20%
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
    // For now, always generate "Sharp" prefix for weapons with seed 1
    if (m_type == ItemType::WEAPON && seed == 1) {
        m_prefixName = "Sharp";
    }
}

std::string Item::getFullName() const {
    if (hasPrefix()) {
        return m_prefixName + " " + m_name;
    }
    return m_name;
}

} // namespace d2::game