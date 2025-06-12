#pragma once

namespace d2::game {

class CombatEngine {
public:
    // Calculate hit chance based on Diablo II formula
    float calculateHitChance(int attackRating, int defense, 
                           int attackerLevel, int defenderLevel) const;
    
    // Calculate damage based on min/max range
    int calculateDamage(int minDamage, int maxDamage, int elementalDamage) const;
};

} // namespace d2::game