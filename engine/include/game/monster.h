#pragma once

#include <cstdint>

namespace d2::game {

enum class MonsterType {
    SKELETON
};

enum class AIState {
    IDLE,
    SEEKING
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
    
    // AI methods
    AIState getAIState() const { return m_aiState; }
    void setTarget(int x, int y);
    void clearTarget();
    void updateAI();
    
private:
    MonsterType m_type;
    int m_level;
    int m_life;
    int m_damage;
    int m_defense;
    int m_attackRating;
    
    // AI state
    AIState m_aiState;
    bool m_hasTarget;
    int m_targetX;
    int m_targetY;
    
    void initializeStats();
};

} // namespace d2::game