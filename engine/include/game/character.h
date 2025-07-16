#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace d2::game {

// Forward declarations 
class Item;
class SkillTree;
class Skill;

/**
 * @brief Enumeration of available character classes in Diablo II
 * 
 * These represent the seven playable character classes from the base game
 * and Lord of Destruction expansion.
 */
enum class CharacterClass {
    BARBARIAN,    ///< Melee warrior with high strength and vitality
    NECROMANCER,  ///< Spellcaster who commands the undead
    AMAZON,       ///< Ranged combat specialist with javelin and bow skills
    SORCERESS,    ///< Elemental magic specialist
    PALADIN,      ///< Holy warrior with auras and combat skills
    ASSASSIN,     ///< Martial arts and trap specialist (expansion)
    DRUID         ///< Shape-shifting nature magic user (expansion)
};

/**
 * @brief Types of character statistics
 * 
 * Core attributes that define a character's capabilities.
 */
enum class StatType {
    STRENGTH,   ///< Affects damage and equipment requirements
    DEXTERITY,  ///< Affects attack rating and defense
    VITALITY,   ///< Affects life points and stamina
    ENERGY,     ///< Affects mana points for spellcasting
    DAMAGE      ///< For item damage bonuses
};

/**
 * @brief Represents a player character in Diablo II
 * 
 * This class encapsulates all aspects of a character including stats,
 * level progression, quest completion, waypoint activation, and equipment.
 * 
 * The character system follows Diablo II mechanics with accurate formulas
 * for life calculation, damage computation, and stat progression.
 * 
 * @example
 * @code
 * // Create a new barbarian character
 * Character barbarian(CharacterClass::BARBARIAN);
 * barbarian.setLevel(10);
 * barbarian.addStatPoint(StatType::STRENGTH, 5);
 * 
 * // Check character stats
 * int life = barbarian.getLife();
 * int damage = barbarian.getBaseDamage();
 * @endcode
 */
class Character {
public:
    explicit Character(CharacterClass characterClass);
    ~Character();
    
    // Make Character movable but not copyable
    Character(const Character&) = delete;
    Character& operator=(const Character&) = delete;
    Character(Character&&) noexcept;
    Character& operator=(Character&&) noexcept;
    
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
    
    // Skill management
    SkillTree* getSkillTree() const;
    Skill* findSkill(const std::string& skillName) const;
    bool addSkillPoint(const std::string& skillName);
    
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
    
    // Skill tree
    std::unique_ptr<SkillTree> m_skillTree;
    
    // Base stats by class
    void initializeBaseStats();
    void checkLevelUp();
};

} // namespace d2::game