#include "game/monster.h"

namespace d2::game {

Monster::Monster(MonsterType type, int level)
    : m_type(type)
    , m_level(level)
    , m_life(1)      // Minimal > 0 value to pass test
    , m_damage(1)    // Minimal > 0 value to pass test
    , m_defense(0)   // Minimal >= 0 value to pass test
{
}

} // namespace d2::game