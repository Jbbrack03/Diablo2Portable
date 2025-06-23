#include <gtest/gtest.h>
#include "performance/memory_monitor.h"
#include <memory>
#include <vector>

using namespace d2;

class MemoryMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        monitor = std::make_unique<MemoryMonitor>();
    }
    
    std::unique_ptr<MemoryMonitor> monitor;
};

TEST_F(MemoryMonitorTest, TrackMemoryAllocation) {
    // Test that we can track memory allocations
    size_t initial_usage = monitor->getCurrentMemoryUsage();
    
    // Allocate 1MB of memory
    const size_t allocation_size = 1024 * 1024; // 1MB
    monitor->recordAllocation("test_allocation", allocation_size);
    
    // Check that memory usage increased
    size_t new_usage = monitor->getCurrentMemoryUsage();
    EXPECT_EQ(new_usage, initial_usage + allocation_size);
}