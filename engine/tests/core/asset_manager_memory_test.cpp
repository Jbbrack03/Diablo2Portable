#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "performance/memory_monitor.h"
#include <memory>
#include <fstream>
#include <filesystem>

using namespace d2portable::core;
using namespace d2;

class AssetManagerMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        assetManager = std::make_unique<AssetManager>();
        memoryMonitor = std::make_unique<MemoryMonitor>();
    }
    
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<MemoryMonitor> memoryMonitor;
};

TEST_F(AssetManagerMemoryTest, SetMemoryMonitor) {
    // Test that AssetManager can use a MemoryMonitor for tracking
    assetManager->setMemoryMonitor(memoryMonitor.get());
    
    // Verify the memory monitor was set
    EXPECT_EQ(assetManager->getMemoryMonitor(), memoryMonitor.get());
}

TEST_F(AssetManagerMemoryTest, TrackClearCacheMemoryUsage) {
    // Test that clearing cache reports memory deallocation to MemoryMonitor
    
    // Initialize with current directory
    const std::string test_data = ".";
    ASSERT_TRUE(assetManager->initialize(test_data));
    
    // Set the memory monitor
    assetManager->setMemoryMonitor(memoryMonitor.get());
    
    // Since we can't load actual sprites in unit tests, we'll just verify
    // that clearCache doesn't crash when memory monitor is set
    assetManager->clearCache();
    
    // Memory usage should still be zero
    EXPECT_EQ(memoryMonitor->getCurrentMemoryUsage(), 0);
}