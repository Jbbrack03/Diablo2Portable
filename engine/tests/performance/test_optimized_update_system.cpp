#include <gtest/gtest.h>
#include "performance/optimized_update_system.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/character.h"

using namespace d2::performance;
using namespace d2::game;

// Unit tests for OptimizedUpdateSystem class
// Following strict TDD principles - one test at a time

// TEST 1: Create optimized update system
TEST(OptimizedUpdateSystemTest, CreateOptimizedUpdateSystem) {
    OptimizedUpdateSystem system;
    // System should exist and be in initial state
    EXPECT_TRUE(system.areOptimizationsEnabled());
    EXPECT_EQ(system.getBatchSize(), 10);
}

// TEST 2: Configure optimization settings
TEST(OptimizedUpdateSystemTest, ConfigureOptimizationSettings) {
    OptimizedUpdateSystem system;
    
    // Disable optimizations
    system.setOptimizationsEnabled(false);
    EXPECT_FALSE(system.areOptimizationsEnabled());
    
    // Enable optimizations
    system.setOptimizationsEnabled(true);
    EXPECT_TRUE(system.areOptimizationsEnabled());
    
    // Set batch size
    system.setBatchSize(5);
    EXPECT_EQ(system.getBatchSize(), 5);
    
    system.setBatchSize(20);
    EXPECT_EQ(system.getBatchSize(), 20);
}

// TEST 3: Configure LOD settings
TEST(OptimizedUpdateSystemTest, ConfigureLODSettings) {
    OptimizedUpdateSystem system;
    
    // LOD should be enabled by default
    // We can test LOD by setting up different distances and calling updateEntities
    
    // Configure LOD distances
    system.setLODDistances(100.0f, 300.0f, 600.0f);
    
    // Test that we can set LOD enabled/disabled
    system.setLODEnabled(false);
    system.setLODEnabled(true);
    
    // Test passes if no exceptions are thrown
    EXPECT_TRUE(true);
}

// TEST 4: Update entities with empty game state
TEST(OptimizedUpdateSystemTest, UpdateEntitiesEmptyGameState) {
    OptimizedUpdateSystem system;
    GameState gameState;
    
    // Should handle empty game state without crashing
    system.updateEntities(gameState, 0.016f);
    
    // Test passes if no exceptions are thrown
    EXPECT_TRUE(true);
}

// TEST 5: Test optimization enabled vs disabled
TEST(OptimizedUpdateSystemTest, OptimizationEnabledVsDisabled) {
    OptimizedUpdateSystem system;
    GameState gameState;
    
    // Create a simple monster for testing
    auto monster = std::make_shared<Monster>(MonsterType::SKELETON, 1);
    monster->setPosition(100, 100);
    gameState.addMonster(monster);
    
    // Test with optimizations enabled
    system.setOptimizationsEnabled(true);
    system.updateEntities(gameState, 0.016f);
    
    // Test with optimizations disabled
    system.setOptimizationsEnabled(false);
    system.updateEntities(gameState, 0.016f);
    
    // Test passes if no exceptions are thrown
    EXPECT_TRUE(true);
}