#pragma once

namespace d2::game {

class CombatEngine {
public:
    // Calculate hit chance based on Diablo II formula
    float calculateHitChance(int attackRating, int defense, 
                           int attackerLevel, int defenderLevel) const;
};

} // namespace d2::game