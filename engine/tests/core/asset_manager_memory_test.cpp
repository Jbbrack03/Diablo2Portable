#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "performance/memory_monitor.h"
#include <memory>

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