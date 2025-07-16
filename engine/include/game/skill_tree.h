#pragma once

#include "game/character.h"
#include "game/skill.h"
#include <vector>
#include <memory>

namespace d2::game {

/**
 * @brief Represents a skill tab/tree within a character class
 * 
 * Each character class has 3 skill tabs in Diablo II:
 * - Sorceress: Fire, Cold, Lightning
 * - Barbarian: Combat, Masteries, Warcries
 * - etc.
 */
class SkillTab {
public:
    SkillTab(const std::string& name);
    
    const std::string& getName() const { return m_name; }
    int getSkillCount() const { return m_skills.size(); }
    
    void addSkill(std::unique_ptr<Skill> skill);
    Skill* getSkill(int index) const;
    
private:
    std::string m_name;
    std::vector<std::unique_ptr<Skill>> m_skills;
};

/**
 * @brief Complete skill tree for a character class
 * 
 * Manages all skill tabs and skills for a specific character class,
 * including prerequisite validation and skill point allocation.
 */
class SkillTree {
public:
    explicit SkillTree(CharacterClass characterClass);
    
    CharacterClass getCharacterClass() const;
    int getSkillTabCount() const;
    int getTotalSkillCount() const;
    
    SkillTab* getSkillTab(int index) const;
    Skill* findSkill(const std::string& skillName) const;
    
private:
    CharacterClass m_characterClass;
    std::vector<std::unique_ptr<SkillTab>> m_skillTabs;
    
    void initializeSkillsForClass();
    void initializeSorceressSkills();
    void initializeBarbarianSkills();
    void initializeAmazonSkills();
    void initializeNecromancerSkills();
    void initializePaladinSkills();
    void initializeAssassinSkills();
    void initializeDruidSkills();
};

} // namespace d2::game