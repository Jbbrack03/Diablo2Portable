#pragma once

#include <string>

namespace d2::game {

enum class SkillType {
    FIRE,
    COLD,
    LIGHTNING,
    POISON,
    PHYSICAL,
    MAGIC
};

class Skill {
public:
    Skill(SkillType type, const std::string& name);
    
    const std::string& getName() const { return m_name; }
    SkillType getType() const { return m_type; }
    int getLevel() const { return m_level; }
    int getMaxLevel() const { return m_maxLevel; }
    
    bool addSkillPoint();
    bool canAddSkillPoint() const;
    void setPrerequisite(Skill* prerequisiteSkill, int requiredLevel);
    
private:
    SkillType m_type;
    std::string m_name;
    int m_level = 0;
    int m_maxLevel = 20;
    
    Skill* m_prerequisiteSkill = nullptr;
    int m_prerequisiteLevel = 0;
};

} // namespace d2::game