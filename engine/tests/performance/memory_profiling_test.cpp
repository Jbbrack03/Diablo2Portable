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