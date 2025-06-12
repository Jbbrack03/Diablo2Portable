#include "game/combat_engine.h"
#include <algorithm>
#include <random>

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

int CombatEngine::calculateDamage(int minDamage, int maxDamage, int elementalDamage) const {
    // Minimal implementation to pass test
    // Generate random damage between min and max
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> dis(minDamage, maxDamage);
    return dis(gen);
}

int CombatEngine::calculateDamageWithResistance(int baseDamage, int physicalResist,
                                               int fireResist, int coldResist,
                                               int lightningResist, int poisonResist) const {
    // Minimal implementation to pass test
    // Apply physical resistance as percentage reduction
    float resistMultiplier = 1.0f - (physicalResist / 100.0f);
    return static_cast<int>(baseDamage * resistMultiplier);
}

int CombatEngine::calculateFullDamage(int physicalDamage, int fireDamage, int coldDamage,
                                    int lightningDamage, int poisonDamage,
                                    int physicalResist, int fireResist, int coldResist,
                                    int lightningResist, int poisonResist) const {
    // Minimal implementation to pass test
    // Calculate damage for each type after applying resistances
    int totalDamage = 0;
    
    // Physical damage
    totalDamage += static_cast<int>(physicalDamage * (1.0f - physicalResist / 100.0f));
    
    // Fire damage
    totalDamage += static_cast<int>(fireDamage * (1.0f - fireResist / 100.0f));
    
    // Cold damage
    totalDamage += static_cast<int>(coldDamage * (1.0f - coldResist / 100.0f));
    
    // Lightning damage
    totalDamage += static_cast<int>(lightningDamage * (1.0f - lightningResist / 100.0f));
    
    // Poison damage
    totalDamage += static_cast<int>(poisonDamage * (1.0f - poisonResist / 100.0f));
    
    return totalDamage;
}

} // namespace d2::game