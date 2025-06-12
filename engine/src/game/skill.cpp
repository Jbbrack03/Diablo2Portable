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

bool Skill::addSynergy(Skill* synergySkill, float bonusPerLevel) {
    if (m_synergies.size() >= MAX_SYNERGIES) {
        return false;
    }
    
    m_synergies.push_back({synergySkill, bonusPerLevel});
    return true;
}

float Skill::getSynergyBonus() const {
    float totalBonus = 0.0f;
    
    for (const auto& [skill, bonusPerLevel] : m_synergies) {
        totalBonus += skill->getLevel() * bonusPerLevel;
    }
    
    return totalBonus;
}

void Skill::setBaseDamage(int minDamage, int maxDamage) {
    m_baseMinDamage = minDamage;
    m_baseMaxDamage = maxDamage;
}

void Skill::setDamagePerLevel(int minPerLevel, int maxPerLevel) {
    m_minDamagePerLevel = minPerLevel;
    m_maxDamagePerLevel = maxPerLevel;
}

int Skill::getMinDamage() const {
    if (m_level == 0) {
        return 0;
    }
    
    int baseDamage = m_baseMinDamage + (m_level - 1) * m_minDamagePerLevel;
    float synergyMultiplier = 1.0f + getSynergyBonus();
    
    return static_cast<int>(baseDamage * synergyMultiplier);
}

int Skill::getMaxDamage() const {
    if (m_level == 0) {
        return 0;
    }
    
    int baseDamage = m_baseMaxDamage + (m_level - 1) * m_maxDamagePerLevel;
    float synergyMultiplier = 1.0f + getSynergyBonus();
    
    return static_cast<int>(baseDamage * synergyMultiplier);
}

} // namespace d2::game