#pragma once

#include <cstdint>

namespace d2::game {

enum class MonsterType {
    SKELETON
};

class Monster {
public:
    explicit Monster(MonsterType type, int level);
    
    MonsterType getType() const { return m_type; }
    int getLevel() const { return m_level; }
    int getLife() const { return m_life; }
    int getDamage() const { return m_damage; }
    int getDefense() const { return m_defense; }
    
private:
    MonsterType m_type;
    int m_level;
    int m_life;
    int m_damage;
    int m_defense;
    
    void initializeStats();
};

} // namespace d2::game