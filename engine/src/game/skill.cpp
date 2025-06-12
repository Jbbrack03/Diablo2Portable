#include "game/skill.h"

namespace d2::game {

Skill::Skill(SkillType type, const std::string& name)
    : m_type(type), m_name(name), m_level(0) {
}

} // namespace d2::game