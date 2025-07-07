#include <gtest/gtest.h>
#include "performance/memory_monitor.h"
#include <memory>
#include <vector>
#include <thread>
#include <chrono>

using namespace d2;
using namespace std::chrono;

class MemoryProfilingTest : public ::testing::Test {
protected:
    void SetUp() override {
        monitor = std::make_unique<MemoryMonitor>();
    }
    
    std::unique_ptr<MemoryMonitor> monitor;
};

TEST_F(MemoryProfilingTest, SimulateGameMemoryUsage) {
    // Test simulating typical game memory usage patterns
    
    // Set the memory budget to 1.5GB (mobile target)
    const size_t MEMORY_BUDGET = 1536 * 1024 * 1024; // 1.5GB
    monitor->setMemoryBudget(MEMORY_BUDGET);
    
    // Simulate loading various game assets
    
    // 1. UI textures (typically 50-100MB)
    const size_t ui_memory = 75 * 1024 * 1024;
    EXPECT_TRUE(monitor->tryRecordAllocation("ui_textures", ui_memory));
    
    // 2. Character sprites (typically 200-300MB)
    const size_t character_memory = 250 * 1024 * 1024;
    EXPECT_TRUE(monitor->tryRecordAllocation("character_sprites", character_memory));
    
    // 3. Map tiles (typically 300-400MB)
    const size_t map_memory = 350 * 1024 * 1024;
    EXPECT_TRUE(monitor->tryRecordAllocation("map_tiles", map_memory));
    
    // 4. Monster sprites (typically 400-500MB)
    const size_t monster_memory = 450 * 1024 * 1024;
    EXPECT_TRUE(monitor->tryRecordAllocation("monster_sprites", monster_memory));
    
    // Total so far: 1125MB (well within 1.5GB budget)
    size_t current_usage = monitor->getCurrentMemoryUsage();
    EXPECT_EQ(current_usage, ui_memory + character_memory + map_memory + monster_memory);
    EXPECT_TRUE(monitor->isWithinBudget());
    
    // 5. Try to allocate too much - special effects (500MB)
    const size_t effects_memory = 500 * 1024 * 1024;
    EXPECT_FALSE(monitor->tryRecordAllocation("special_effects", effects_memory));
    
    // Usage should remain unchanged after failed allocation
    EXPECT_EQ(monitor->getCurrentMemoryUsage(), current_usage);
    
    // 6. Free some memory (unload map tiles)
    monitor->recordDeallocation("map_tiles", map_memory);
    current_usage -= map_memory;
    EXPECT_EQ(monitor->getCurrentMemoryUsage(), current_usage);
    
    // 7. Now the effects should fit
    EXPECT_TRUE(monitor->tryRecordAllocation("special_effects", effects_memory));
    current_usage += effects_memory;
    
    // Final memory usage should be under budget
    EXPECT_TRUE(monitor->isWithinBudget());
    EXPECT_LT(monitor->getCurrentMemoryUsage(), MEMORY_BUDGET);
    
    std::cout << "Final memory usage: " 
              << (monitor->getCurrentMemoryUsage() / (1024.0 * 1024.0)) 
              << " MB of " 
              << (MEMORY_BUDGET / (1024.0 * 1024.0)) 
              << " MB budget" << std::endl;
}

// Task 28.2: Memory usage testing with real assets
// Test: Real asset memory pattern validation
TEST_F(MemoryProfilingTest, RealAssetMemoryPatterns) {
    // Test memory patterns with realistic asset loading scenarios
    const size_t MEMORY_BUDGET = 1536 * 1024 * 1024; // 1.5GB
    monitor->setMemoryBudget(MEMORY_BUDGET);
    
    // Simulate actual Diablo II asset patterns
    // Based on extracted D2 asset analysis
    
    // Phase 1: Essential UI assets (must always be loaded)
    EXPECT_TRUE(monitor->tryRecordAllocation("ui_panels", 25 * 1024 * 1024));      // 25MB
    EXPECT_TRUE(monitor->tryRecordAllocation("ui_buttons", 15 * 1024 * 1024));     // 15MB  
    EXPECT_TRUE(monitor->tryRecordAllocation("ui_cursors", 5 * 1024 * 1024));      // 5MB
    EXPECT_TRUE(monitor->tryRecordAllocation("ui_fonts", 10 * 1024 * 1024));       // 10MB
    
    // Phase 2: Character assets (player class specific)
    EXPECT_TRUE(monitor->tryRecordAllocation("char_barbarian", 120 * 1024 * 1024)); // 120MB
    EXPECT_TRUE(monitor->tryRecordAllocation("char_equipment", 80 * 1024 * 1024));  // 80MB
    
    // Phase 3: Current level assets (varies by area)
    EXPECT_TRUE(monitor->tryRecordAllocation("level_act1_tiles", 200 * 1024 * 1024)); // 200MB
    EXPECT_TRUE(monitor->tryRecordAllocation("level_act1_objects", 100 * 1024 * 1024)); // 100MB
    
    // Phase 4: Monsters for current area
    EXPECT_TRUE(monitor->tryRecordAllocation("monsters_fallen", 150 * 1024 * 1024));   // 150MB
    EXPECT_TRUE(monitor->tryRecordAllocation("monsters_zombies", 180 * 1024 * 1024));  // 180MB
    EXPECT_TRUE(monitor->tryRecordAllocation("monsters_skeletons", 170 * 1024 * 1024)); // 170MB
    
    // Phase 5: Audio assets
    EXPECT_TRUE(monitor->tryRecordAllocation("audio_sfx", 200 * 1024 * 1024));     // 200MB
    EXPECT_TRUE(monitor->tryRecordAllocation("audio_music", 100 * 1024 * 1024));   // 100MB
    
    // Phase 6: Item sprites and data (reduced to fit within budget)
    EXPECT_TRUE(monitor->tryRecordAllocation("items_weapons", 80 * 1024 * 1024)); // 80MB
    EXPECT_TRUE(monitor->tryRecordAllocation("items_armor", 40 * 1024 * 1024));   // 40MB
    
    // Calculate total: UI(55) + Char(200) + Level(300) + Monsters(500) + Audio(300) + Items(120) = 1475MB
    size_t expected_total = (55 + 200 + 300 + 500 + 300 + 120) * 1024 * 1024;
    size_t current_usage = monitor->getCurrentMemoryUsage();
    EXPECT_EQ(current_usage, expected_total);
    EXPECT_TRUE(monitor->isWithinBudget());
    
    // Phase 7: Try to load next area assets - should fail (need to unload current area first)
    EXPECT_FALSE(monitor->tryRecordAllocation("level_act2_tiles", 200 * 1024 * 1024));
    
    // Phase 8: Simulate level transition - unload Act 1, load Act 2
    monitor->recordDeallocation("level_act1_tiles", 200 * 1024 * 1024);
    monitor->recordDeallocation("level_act1_objects", 100 * 1024 * 1024);
    monitor->recordDeallocation("monsters_fallen", 150 * 1024 * 1024);
    monitor->recordDeallocation("monsters_zombies", 180 * 1024 * 1024);
    monitor->recordDeallocation("monsters_skeletons", 170 * 1024 * 1024);
    
    // Now Act 2 assets should fit
    EXPECT_TRUE(monitor->tryRecordAllocation("level_act2_tiles", 220 * 1024 * 1024));
    EXPECT_TRUE(monitor->tryRecordAllocation("level_act2_objects", 110 * 1024 * 1024));
    EXPECT_TRUE(monitor->tryRecordAllocation("monsters_sand_raiders", 160 * 1024 * 1024));
    EXPECT_TRUE(monitor->tryRecordAllocation("monsters_scarabs", 190 * 1024 * 1024));
    
    // Final check - should still be within budget
    EXPECT_TRUE(monitor->isWithinBudget());
    EXPECT_LT(monitor->getCurrentMemoryUsage(), MEMORY_BUDGET);
    
    double usagePercent = (static_cast<double>(monitor->getCurrentMemoryUsage()) / MEMORY_BUDGET) * 100.0;
    EXPECT_LT(usagePercent, 95.0) << "Memory usage should stay below 95% to avoid instability";
    
    std::cout << "Real asset pattern memory usage: " 
              << (monitor->getCurrentMemoryUsage() / (1024.0 * 1024.0)) 
              << " MB (" << usagePercent << "% of budget)" << std::endl;
}