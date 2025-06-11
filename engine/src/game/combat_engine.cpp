#include "game/combat_engine.h"
#include <algorithm>

namespace d2::game {

float CombatEngine::calculateHitChance(int attackRating, int defense, 
                                      int attackerLevel, int defenderLevel) const {
    // Minimal implementation to pass test
    // Based on Diablo II hit chance formula:
    // Chance to Hit = 2 * AR / (AR + DEF) * AL / (AL + DL)
    
    // Avoid division by zero
    if (attackRating + defense == 0) return 0.0f;
    if (attackerLevel + defenderLevel == 0) return 0.0f;
    
    float arFactor = 2.0f * attackRating / (attackRating + defense);
    float levelFactor = static_cast<float>(attackerLevel) / (attackerLevel + defenderLevel);
    
    float hitChance = arFactor * levelFactor;
    
    // Clamp between 0 and 1 (0% to 100%)
    return std::clamp(hitChance, 0.0f, 1.0f);
}

} // namespace d2::game