#pragma once

#include <string>
#include <vector>
#include <utility>

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
    
    bool addSynergy(Skill* synergySkill, float bonusPerLevel);
    float getSynergyBonus() const;
    
    void setBaseDamage(int minDamage, int maxDamage);
    void setDamagePerLevel(int minPerLevel, int maxPerLevel);
    int getMinDamage() const;
    int getMaxDamage() const;
    
    void setBaseManaCost(float manaCost);
    void setManaCostReduction(float reductionPerLevel);
    float getManaCost() const;
    
private:
    SkillType m_type;
    std::string m_name;
    int m_level = 0;
    int m_maxLevel = 20;
    
    Skill* m_prerequisiteSkill = nullptr;
    int m_prerequisiteLevel = 0;
    
    static constexpr int MAX_SYNERGIES = 3;
    std::vector<std::pair<Skill*, float>> m_synergies;
    
    int m_baseMinDamage = 0;
    int m_baseMaxDamage = 0;
    int m_minDamagePerLevel = 0;
    int m_maxDamagePerLevel = 0;
    
    float m_baseManaCost = 0.0f;
    float m_manaCostReduction = 0.0f;
};

} // namespace d2::game