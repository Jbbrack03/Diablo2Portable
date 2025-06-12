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
    
private:
    SkillType m_type;
    std::string m_name;
    int m_level = 0;
};

} // namespace d2::game