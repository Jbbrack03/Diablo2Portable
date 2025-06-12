#include "game/item.h"

namespace d2::game {

Item::Item(const std::string& name, ItemType type)
    : m_name(name), m_type(type) {
}

void Item::setDamage(int minDamage, int maxDamage) {
    m_minDamage = minDamage;
    m_maxDamage = maxDamage;
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

} // namespace d2::game