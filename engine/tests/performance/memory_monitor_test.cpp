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

TEST_F(MemoryMonitorTest, TrackMemoryDeallocation) {
    // Test that we can track memory deallocations
    const size_t allocation_size = 1024 * 1024; // 1MB
    monitor->recordAllocation("test_allocation", allocation_size);
    
    size_t after_alloc = monitor->getCurrentMemoryUsage();
    EXPECT_EQ(after_alloc, allocation_size);
    
    // Deallocate the memory
    monitor->recordDeallocation("test_allocation", allocation_size);
    
    // Check that memory usage decreased
    size_t after_dealloc = monitor->getCurrentMemoryUsage();
    EXPECT_EQ(after_dealloc, 0);
}

TEST_F(MemoryMonitorTest, EnforceMemoryBudget) {
    // Test that we can enforce a memory budget (1.5GB for mobile)
    const size_t budget = 1536 * 1024 * 1024; // 1.5GB
    monitor->setMemoryBudget(budget);
    
    // Check initial budget
    EXPECT_EQ(monitor->getMemoryBudget(), budget);
    EXPECT_TRUE(monitor->isWithinBudget());
    
    // Allocate within budget
    const size_t small_alloc = 512 * 1024 * 1024; // 512MB
    monitor->recordAllocation("small_alloc", small_alloc);
    EXPECT_TRUE(monitor->isWithinBudget());
    
    // Try to allocate beyond budget
    const size_t large_alloc = 1200 * 1024 * 1024; // 1.2GB
    bool result = monitor->tryRecordAllocation("large_alloc", large_alloc);
    EXPECT_FALSE(result); // Should fail as it would exceed budget
    EXPECT_EQ(monitor->getCurrentMemoryUsage(), small_alloc); // Usage unchanged
}