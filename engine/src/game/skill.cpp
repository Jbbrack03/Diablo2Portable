#include "game/skill.h"

namespace d2::game {

Skill::Skill(SkillType type, const std::string& name)
    : m_type(type), m_name(name), m_level(0), m_maxLevel(20) {
}

bool Skill::addSkillPoint() {
    if (m_level >= m_maxLevel) {
        return false;
    }
    m_level++;
    return true;
}

} // namespace d2::game