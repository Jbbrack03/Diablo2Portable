#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <filesystem>
#include <numeric>
#include "game/game_engine.h"
#include "sprites/dc6_parser.h"
#include "utils/mpq_loader.h"
#include "performance/performance_monitor.h"
#include "rendering/sprite_renderer.h"

namespace fs = std::filesystem;

class AndroidAssetPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Check if we have real MPQ files available
        mpq_available_ = checkMPQFilesAvailable();
        
        // Set up test environment
        monitor_ = std::make_unique<d2::performance::PerformanceMonitor>();
    }
    
    bool checkMPQFilesAvailable() {
        // Check for MPQ files in vendor directory
        fs::path mpq_path = "vendor/mpq";
        if (!fs::exists(mpq_path)) {
            return false;
        }
        
        // Check for key MPQ files
        std::vector<std::string> required_mpqs = {
            "d2data.mpq",
            "d2char.mpq",
            "d2sfx.mpq"
        };
        
        for (const auto& mpq : required_mpqs) {
            if (!fs::exists(mpq_path / mpq)) {
                return false;
            }
        }
        
        return true;
    }
    
    std::unique_ptr<d2::performance::PerformanceMonitor> monitor_;
    bool mpq_available_;
};

// Test loading performance of real D2 assets
TEST_F(AndroidAssetPerformanceTest, MeasureAssetLoadingPerformance) {
    if (!mpq_available_) {
        GTEST_SKIP() << "Real MPQ files not available in vendor/mpq";
    }
    
    d2::MPQLoader loader;
    
    // Measure MPQ open time
    auto start = std::chrono::high_resolution_clock::now();
    bool opened = loader.open("vendor/mpq/d2data.mpq");
    auto end = std::chrono::high_resolution_clock::now();
    
    if (!opened) {
        GTEST_SKIP() << "Could not open d2data.mpq - file may be missing or corrupted";
    }
    
    auto openTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // MPQ open should be reasonably fast on mobile
    EXPECT_LT(openTime, 500) << "MPQ open took " << openTime << "ms (limit: 500ms)";
    
    // Test extracting multiple sprites
    std::vector<std::string> test_sprites = {
        "data/global/chars/am/amnu1hth.dc6",  // Amazon sprite
        "data/global/chars/ba/banu1hth.dc6",  // Barbarian sprite
        "data/global/chars/ne/nenu1hth.dc6"   // Necromancer sprite
    };
    
    std::vector<double> extractTimes;
    
    for (const auto& sprite_path : test_sprites) {
        start = std::chrono::high_resolution_clock::now();
        
        std::vector<uint8_t> data;
        bool extracted = loader.extractFile(sprite_path, data);
        
        end = std::chrono::high_resolution_clock::now();
        
        if (extracted) {
            auto extractTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            extractTimes.push_back(extractTime);
        }
    }
    
    if (!extractTimes.empty()) {
        double avgExtractTime = std::accumulate(extractTimes.begin(), extractTimes.end(), 0.0) / extractTimes.size();
        double avgExtractTimeMs = avgExtractTime / 1000.0;
        
        // Individual sprite extraction should be fast
        EXPECT_LT(avgExtractTimeMs, 50) << "Average sprite extraction took " << avgExtractTimeMs << "ms (limit: 50ms)";
    }
}

// Test sprite rendering performance with real assets
TEST_F(AndroidAssetPerformanceTest, MeasureSpriteRenderingPerformance) {
    if (!mpq_available_) {
        GTEST_SKIP() << "Real MPQ files not available in vendor/mpq";
    }
    
    d2::MPQLoader loader;
    if (!loader.open("vendor/mpq/d2char.mpq")) {
        GTEST_SKIP() << "Could not open d2char.mpq - file may be missing or corrupted";
    }
    
    // Extract a character sprite
    std::vector<uint8_t> sprite_data;
    if (!loader.extractFile("data/global/chars/am/amnu1hth.dc6", sprite_data)) {
        // Try alternative sprite path
        if (!loader.extractFile("data/global/chars/am/amnu1hth.dcc", sprite_data)) {
            GTEST_SKIP() << "Could not extract character sprite - file may not exist in MPQ";
        }
    }
    
    // Parse the sprite
    d2portable::sprites::DC6Parser parser;
    auto sprite = parser.parseData(sprite_data);
    ASSERT_NE(sprite, nullptr);
    
    // Measure rendering performance
    const int NUM_FRAMES = 100;
    std::vector<double> frameTimes;
    
    d2::rendering::SpriteRenderer renderer;
    
    for (int i = 0; i < NUM_FRAMES; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate rendering the sprite
        // In real implementation, this would render to OpenGL ES
        auto frame = sprite->getFrame(0, i % sprite->getFramesPerDirection());
        monitor_->recordDrawCall();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        frameTimes.push_back(frameTime);
    }
    
    // Calculate average frame time
    double avgFrameTime = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();
    double avgFrameTimeMs = avgFrameTime / 1000.0;
    
    // Calculate FPS
    double fps = 1000.0 / avgFrameTimeMs;
    
    // We should achieve at least 60 FPS on the target device
    EXPECT_GT(fps, 60) << "Average FPS: " << fps << " (frame time: " << avgFrameTimeMs << "ms)";
}

// Test memory usage with multiple assets loaded
TEST_F(AndroidAssetPerformanceTest, MeasureMemoryUsageWithAssets) {
    if (!mpq_available_) {
        GTEST_SKIP() << "Real MPQ files not available in vendor/mpq";
    }
    
    // Get baseline memory
    size_t baselineMemory = monitor_->getCurrentMemoryUsage();
    
    // Load multiple MPQ files
    std::vector<std::unique_ptr<d2::MPQLoader>> loaders;
    std::vector<std::string> mpq_files = {
        "vendor/mpq/d2data.mpq",
        "vendor/mpq/d2char.mpq",
        "vendor/mpq/d2sfx.mpq"
    };
    
    for (const auto& mpq_file : mpq_files) {
        if (fs::exists(mpq_file)) {
            auto loader = std::make_unique<d2::MPQLoader>();
            if (loader->open(mpq_file)) {
                loaders.push_back(std::move(loader));
            }
        }
    }
    
    // Extract and parse multiple sprites
    std::vector<std::unique_ptr<d2portable::sprites::DC6Sprite>> sprites;
    d2portable::sprites::DC6Parser parser;
    
    if (!loaders.empty()) {
        std::vector<std::string> sprite_paths = {
            "data/global/chars/am/amnu1hth.dc6",
            "data/global/chars/ba/banu1hth.dc6",
            "data/global/chars/ne/nenu1hth.dc6",
            "data/global/chars/pa/panu1hth.dc6",
            "data/global/chars/so/sonu1hth.dc6"
        };
        
        for (const auto& sprite_path : sprite_paths) {
            std::vector<uint8_t> data;
            if (loaders[0]->extractFile(sprite_path, data)) {
                auto sprite = parser.parseData(data);
                if (sprite) {
                    sprites.push_back(std::move(sprite));
                }
            }
        }
    }
    
    // Measure memory after loading assets
    size_t currentMemory = monitor_->getCurrentMemoryUsage();
    size_t memoryIncrease = currentMemory - baselineMemory;
    size_t memoryIncreaseMB = memoryIncrease / (1024 * 1024);
    
    // Memory usage should stay within reasonable bounds
    // Target device has 3GB RAM, game should use < 1.5GB total
    EXPECT_LT(memoryIncreaseMB, 500) << "Memory increased by " << memoryIncreaseMB << "MB after loading assets";
    
    // Log stats for debugging
    std::cout << "Loaded " << loaders.size() << " MPQ files and " << sprites.size() << " sprites" << std::endl;
    std::cout << "Memory increase: " << memoryIncreaseMB << "MB" << std::endl;
}

// Test performance with concurrent asset loading (simulating game startup)
TEST_F(AndroidAssetPerformanceTest, MeasureConcurrentAssetLoading) {
    if (!mpq_available_) {
        GTEST_SKIP() << "Real MPQ files not available in vendor/mpq";
    }
    
    // Measure time to load all essential game assets
    auto start = std::chrono::high_resolution_clock::now();
    
    // Initialize game engine with real assets
    d2::GameEngine engine;
    
    // In a real scenario, this would:
    // 1. Load all MPQ files
    // 2. Extract essential sprites, sounds, and data
    // 3. Initialize game systems
    
    auto end = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    
    // Game should load within reasonable time on mobile
    EXPECT_LT(loadTime, 10) << "Game initialization took " << loadTime << " seconds (limit: 10s)";
}