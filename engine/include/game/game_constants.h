#pragma once

#include <cstddef>

/**
 * @file game_constants.h
 * @brief Game-wide constants for Diablo II mechanics
 * 
 * This file contains all magic numbers and configuration values used
 * throughout the Diablo II game engine. By centralizing these values,
 * we improve maintainability and reduce the chance of inconsistencies.
 */

namespace d2::game {
namespace constants {

// Waypoint indices for town waypoints (always active)
constexpr int WAYPOINT_ROGUE_ENCAMPMENT = 0;    // Act 1
constexpr int WAYPOINT_LUT_GHOLEIN = 9;         // Act 2  
constexpr int WAYPOINT_KURAST_DOCKS = 18;       // Act 3
constexpr int WAYPOINT_PANDEMONIUM_FORTRESS = 27; // Act 4
constexpr int WAYPOINT_HARROGATH = 30;          // Act 5 (expansion)

// Character stat calculation constants
constexpr int VITALITY_TO_LIFE_MULTIPLIER = 4;
constexpr float STRENGTH_DAMAGE_BONUS_PERCENT = 0.01f; // 1% per point

// Damage calculation constants
constexpr int BASE_DAMAGE = 15;
constexpr float STRENGTH_DAMAGE_DIVISOR = 2.5f;

// Character life calculation constants per class
namespace life_stats {
    // Barbarian
    constexpr int BARBARIAN_BASE_LIFE = 55;
    constexpr int BARBARIAN_LIFE_PER_LEVEL = 2;
    constexpr int BARBARIAN_LIFE_PER_VITALITY = 4;
    
    // Sorceress
    constexpr int SORCERESS_BASE_LIFE = 40;
    constexpr int SORCERESS_LIFE_PER_LEVEL = 1;
    constexpr int SORCERESS_LIFE_PER_VITALITY = 2;
    
    // Necromancer
    constexpr int NECROMANCER_BASE_LIFE = 45;
    constexpr int NECROMANCER_LIFE_PER_LEVEL = 1; // Actually 1.5, but using int for now
    constexpr int NECROMANCER_LIFE_PER_VITALITY = 2;
    
    // Default values for other classes
    constexpr int DEFAULT_BASE_LIFE = 50;
    constexpr int DEFAULT_LIFE_PER_LEVEL = 2;
    constexpr int DEFAULT_LIFE_PER_VITALITY = 3;
}

// Combat constants
constexpr int MIN_HIT_CHANCE_PERCENT = 5;
constexpr int MAX_HIT_CHANCE_PERCENT = 95;

// Level and progression constants
constexpr int BASE_CHARACTER_LEVEL = 1;
constexpr int MAX_CHARACTER_LEVEL = 99;

// Memory management constants
constexpr size_t DEFAULT_MEMORY_BUDGET_MB = 1536; // 1.5GB in MB

// Performance constants
constexpr int TARGET_FPS = 60;
constexpr double TARGET_FRAME_TIME_MS = 16.67; // 1000ms / 60fps

} // namespace constants
} // namespace d2::game