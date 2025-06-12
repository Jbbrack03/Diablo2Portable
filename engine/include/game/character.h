#pragma once

#include <cstdint>

namespace d2::game {

enum class CharacterClass {
    BARBARIAN,
    NECROMANCER,
    AMAZON,
    SORCERESS,
    PALADIN,
    ASSASSIN,
    DRUID
};

enum class StatType {
    STRENGTH,
    DEXTERITY,
    VITALITY,
    ENERGY,
    DAMAGE  // For item damage bonuses
};

class Character {
public:
    explicit Character(CharacterClass characterClass);
    
    void setLevel(int level);
    void addStatPoint(StatType stat, int points);
    void addExperience(int experience);
    
    int getBaseDamage() const;
    int getLife() const;
    int getLevel() const { return m_level; }
    int getStatPoints() const { return m_statPoints; }
    int getSkillPoints() const { return m_skillPoints; }
    
private:
    CharacterClass m_class;
    int m_level = 1;
    int m_experience = 0;
    int m_statPoints = 0;
    int m_skillPoints = 0;
    
    // Base stats
    int m_strength = 0;
    int m_dexterity = 0;
    int m_vitality = 0;
    int m_energy = 0;
    
    // Base stats by class
    void initializeBaseStats();
    void checkLevelUp();
};

} // namespace d2::game