#include <gtest/gtest.h>
#include "performance/performance_monitor.h"
#include <thread>
#include <chrono>

using namespace d2::performance;

// Unit tests for PerformanceMonitor class
// Following strict TDD principles - one test at a time

// TEST 1: Create performance monitor
TEST(PerformanceMonitorTest, CreatePerformanceMonitor) {
    PerformanceMonitor monitor;
    // Monitor should exist and be in initial state
    EXPECT_EQ(monitor.getCurrentFPS(), 0.0);
    EXPECT_EQ(monitor.getFrameTime(), 0.0);
}

// TEST 2: Measure frame timing
TEST(PerformanceMonitorTest, MeasureFrameTiming) {
    PerformanceMonitor monitor;
    
    // Start frame
    monitor.startFrame();
    
    // Simulate some work (10ms)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // End frame
    monitor.endFrame();
    
    // Frame time should be approximately 10ms
    double frameTime = monitor.getFrameTime();
    EXPECT_GT(frameTime, 9.0);  // At least 9ms
    EXPECT_LT(frameTime, 15.0); // Less than 15ms (accounting for timing variations)
    
    // FPS should be approximately 100 (1000ms / 10ms)
    double fps = monitor.getCurrentFPS();
    EXPECT_GT(fps, 65.0);   // At least 65 FPS
    EXPECT_LT(fps, 115.0);  // Less than 115 FPS
}

// TEST 3: Calculate average FPS over multiple frames
TEST(PerformanceMonitorTest, CalculateAverageFPS) {
    PerformanceMonitor monitor;
    
    // Simulate 10 frames with consistent timing
    for (int i = 0; i < 10; i++) {
        monitor.startFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        monitor.endFrame();
    }
    
    // Average FPS should be approximately 60
    double avgFPS = monitor.getAverageFPS();
    EXPECT_GT(avgFPS, 45.0);  // More forgiving lower bound
    EXPECT_LT(avgFPS, 70.0);  // More forgiving upper bound
    
    // Average frame time should be approximately 16ms
    double avgFrameTime = monitor.getAverageFrameTime();
    EXPECT_GT(avgFrameTime, 14.0);  // More forgiving lower bound
    EXPECT_LT(avgFrameTime, 23.0);  // More forgiving upper bound
}

// TEST 4: Track min and max FPS
TEST(PerformanceMonitorTest, TrackMinMaxFPS) {
    PerformanceMonitor monitor;
    
    // Frame 1: Fast frame (5ms = ~200 FPS)
    monitor.startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    monitor.endFrame();
    
    // Frame 2: Normal frame (16ms = ~60 FPS)  
    monitor.startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
    monitor.endFrame();
    
    // Frame 3: Slow frame (33ms = ~30 FPS)
    monitor.startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
    monitor.endFrame();
    
    // Min FPS should be around 30
    double minFPS = monitor.getMinFPS();
    EXPECT_GT(minFPS, 25.0);
    EXPECT_LT(minFPS, 35.0);
    
    // Max FPS should be around 200
    double maxFPS = monitor.getMaxFPS();
    EXPECT_GT(maxFPS, 150.0);
    EXPECT_LT(maxFPS, 250.0);
}

// TEST 5: Reset statistics
TEST(PerformanceMonitorTest, ResetStatistics) {
    PerformanceMonitor monitor;
    
    // Record some frames
    for (int i = 0; i < 5; i++) {
        monitor.startFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        monitor.endFrame();
    }
    
    // Verify we have data
    EXPECT_GT(monitor.getAverageFPS(), 0.0);
    
    // Reset
    monitor.reset();
    
    // Should be back to initial state
    EXPECT_EQ(monitor.getCurrentFPS(), 0.0);
    EXPECT_EQ(monitor.getAverageFPS(), 0.0);
    EXPECT_EQ(monitor.getFrameTime(), 0.0);
    EXPECT_EQ(monitor.getAverageFrameTime(), 0.0);
}

// TEST 6: Frame history size management
TEST(PerformanceMonitorTest, FrameHistorySize) {
    PerformanceMonitor monitor;
    
    // Set history size to 3 frames
    monitor.setFrameHistorySize(3);
    
    // Record 5 frames with different timings
    monitor.startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    monitor.endFrame();
    
    monitor.startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    monitor.endFrame();
    
    monitor.startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    monitor.endFrame();
    
    monitor.startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    monitor.endFrame();
    
    monitor.startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    monitor.endFrame();
    
    // Average should only include last 3 frames (30ms, 40ms, 50ms)
    // Average = (30 + 40 + 50) / 3 = 40ms
    double avgFrameTime = monitor.getAverageFrameTime();
    EXPECT_GT(avgFrameTime, 35.0);  // More forgiving lower bound
    EXPECT_LT(avgFrameTime, 50.0);  // More forgiving upper bound
}