#pragma once

namespace d2::game {

class CombatEngine {
public:
    // Calculate hit chance based on Diablo II formula
    float calculateHitChance(int attackRating, int defense, 
                           int attackerLevel, int defenderLevel) const;
    
    // Calculate damage based on min/max range
    int calculateDamage(int minDamage, int maxDamage, int elementalDamage) const;
    
    // Calculate damage with resistance reduction
    int calculateDamageWithResistance(int baseDamage, int physicalResist,
                                    int fireResist, int coldResist,
                                    int lightningResist, int poisonResist) const;
    
    // Calculate full damage with all damage types and resistances
    int calculateFullDamage(int physicalDamage, int fireDamage, int coldDamage,
                          int lightningDamage, int poisonDamage,
                          int physicalResist, int fireResist, int coldResist,
                          int lightningResist, int poisonResist) const;
};

} // namespace d2::game