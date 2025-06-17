#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace d2::game {

enum class MonsterType {
    SKELETON,
    ZOMBIE,
    DEMON,
    FALLEN,
    GOLEM
};

enum class AIState {
    IDLE,
    SEEKING,
    PATROLLING,
    ATTACKING,
    FLEEING
};

struct SpecialAbility {
    std::string name;
    int cooldown;
    int range;
    std::string effect;
};

class Monster {
public:
    explicit Monster(MonsterType type, int level);
    
    MonsterType getType() const { return m_type; }
    int getLevel() const { return m_level; }
    int getLife() const { return m_life; }
    int getDamage() const { return m_damage; }
    int getDefense() const { return m_defense; }
    int getAttackRating() const { return m_attackRating; }
    
    // Position methods
    int getPositionX() const { return m_positionX; }
    int getPositionY() const { return m_positionY; }
    void setPosition(int x, int y);
    
    // Combat methods
    void takeDamage(int damage);
    int getCurrentLife() const { return m_currentLife; }
    
    // AI methods
    AIState getAIState() const { return m_aiState; }
    void setTarget(int x, int y);
    void clearTarget();
    void startPatrolling(int centerX, int centerY);
    void updateAI();
    
    // Advanced AI features
    void setTerritoryCenter(int x, int y, int radius);
    bool hasTerritory() const { return m_hasTerritory; }
    int getTerritoryRadius() const { return m_territoryRadius; }
    
    // Elite monster features
    void setEliteType(const std::string& eliteType);
    bool isElite() const { return m_isElite; }
    std::string getEliteType() const { return m_eliteType; }
    std::vector<SpecialAbility> getSpecialAbilities() const { return m_specialAbilities; }
    
    // Sleep/awakening features
    void setSleeping(bool sleeping) { m_isSleeping = sleeping; }
    bool isSleeping() const { return m_isSleeping; }
    void checkPlayerProximity(int playerX, int playerY, float wakeRange);
    
private:
    MonsterType m_type;
    int m_level;
    int m_life;
    int m_currentLife;
    int m_damage;
    int m_defense;
    int m_attackRating;
    
    // Position
    int m_positionX;
    int m_positionY;
    
    // AI state
    AIState m_aiState;
    bool m_hasTarget;
    int m_targetX;
    int m_targetY;
    
    // Patrol state
    bool m_isPatrolling;
    int m_patrolCenterX;
    int m_patrolCenterY;
    
    // Territory system
    bool m_hasTerritory = false;
    int m_territoryCenterX = 0;
    int m_territoryCenterY = 0;
    int m_territoryRadius = 0;
    
    // Elite system
    bool m_isElite = false;
    std::string m_eliteType;
    std::vector<SpecialAbility> m_specialAbilities;
    
    // Sleep system
    bool m_isSleeping = false;
    
    void initializeStats();
    void initializeEliteStats();
};

class MonsterSpawner {
public:
    std::unique_ptr<Monster> spawnMonster(MonsterType type, int level, int x, int y);
};

class MonsterGroup {
public:
    int addMonster(std::unique_ptr<Monster> monster);
    Monster* getMonster(int monsterId);
    void setGroupTarget(int monsterId, int targetX, int targetY);
    void updateGroupAI();

private:
    std::unordered_map<int, std::unique_ptr<Monster>> m_monsters;
    int m_nextMonsterId = 1;
};

} // namespace d2::game