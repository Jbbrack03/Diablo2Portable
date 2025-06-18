#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace d2::game {

// Forward declaration  
class Item;

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
    void setStat(StatType stat, int value);  // For testing
    void addExperience(int experience);
    
    // Stat management
    void setStatPoints(int points) { m_statPoints = points; }
    void setSkillPoints(int points) { m_skillPoints = points; }
    
    // Stat getters
    int getBaseDamage() const;
    int getLife() const;
    int getLevel() const { return m_level; }
    int getStatPoints() const { return m_statPoints; }
    int getSkillPoints() const { return m_skillPoints; }
    CharacterClass getCharacterClass() const { return m_class; }
    
    // Individual stat getters
    int getStrength() const { return m_strength; }
    int getDexterity() const { return m_dexterity; }
    int getVitality() const { return m_vitality; }
    int getEnergy() const { return m_energy; }
    
    // Equipment methods  
    void equipWeapon(std::shared_ptr<Item> weapon);
    std::shared_ptr<Item> getEquippedWeapon() const { return m_equippedWeapon; }
    
    // Damage bonus calculations
    float getStrengthDamageBonus() const;
    
    // Quest management
    void setQuestComplete(int questId, bool complete);
    bool isQuestComplete(int questId) const;
    
    // Waypoint management
    void activateWaypoint(int waypointId);
    bool isWaypointActive(int waypointId) const;
    
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
    
    // Quest progress storage
    // Diablo II has 41 quests total (6 per act in Acts 1-4, 6 in Act 5 expansion)
    // For simplicity, we'll track quest completion as a bit vector
    std::vector<bool> m_questProgress;
    static constexpr int MAX_QUESTS = 41;
    
    // Waypoint activation storage
    // Diablo II has 39 waypoints total across all acts
    // Act 1: 9, Act 2: 9, Act 3: 9, Act 4: 3, Act 5: 9
    std::vector<bool> m_waypointProgress;
    static constexpr int MAX_WAYPOINTS = 39;
    
    // Equipment
    std::shared_ptr<Item> m_equippedWeapon;
    
    // Base stats by class
    void initializeBaseStats();
    void checkLevelUp();
};

} // namespace d2::game