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
    ENERGY
};

class Character {
public:
    explicit Character(CharacterClass characterClass);
    
    void setLevel(int level);
    void addStatPoint(StatType stat, int points);
    
    int getBaseDamage() const;
    int getLife() const;
    
private:
    CharacterClass m_class;
    int m_level = 1;
    
    // Base stats
    int m_strength = 0;
    int m_dexterity = 0;
    int m_vitality = 0;
    int m_energy = 0;
    
    // Base stats by class
    void initializeBaseStats();
};

} // namespace d2::game