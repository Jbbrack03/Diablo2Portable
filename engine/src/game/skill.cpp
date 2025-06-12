#include "game/skill.h"

namespace d2::game {

Skill::Skill(SkillType type, const std::string& name)
    : m_type(type), m_name(name), m_level(0), m_maxLevel(20) {
}

bool Skill::addSkillPoint() {
    if (!canAddSkillPoint()) {
        return false;
    }
    m_level++;
    return true;
}

bool Skill::canAddSkillPoint() const {
    // Check max level
    if (m_level >= m_maxLevel) {
        return false;
    }
    
    // Check prerequisites
    if (m_prerequisiteSkill != nullptr) {
        if (m_prerequisiteSkill->getLevel() < m_prerequisiteLevel) {
            return false;
        }
    }
    
    return true;
}

void Skill::setPrerequisite(Skill* prerequisiteSkill, int requiredLevel) {
    m_prerequisiteSkill = prerequisiteSkill;
    m_prerequisiteLevel = requiredLevel;
}

} // namespace d2::game