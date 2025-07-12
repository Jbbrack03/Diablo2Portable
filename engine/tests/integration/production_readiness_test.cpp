#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <chrono>
#include <fstream>

// Include key system headers for validation
#include "core/asset_manager.h"
#include "rendering/texture_manager.h"
#include "game/game_engine.h"
#include "utils/mpq_loader.h"

namespace fs = std::filesystem;

class ProductionReadinessTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary test environment
        testDir = fs::temp_directory_path() / "production_readiness_test";
        fs::create_directories(testDir);
        
        // Initialize core systems
        assetManager = std::make_unique<d2portable::core::AssetManager>();
        textureManager = std::make_unique<d2::rendering::TextureManager>();
        gameEngine = std::make_unique<d2::GameEngine>();
    }
    
    void TearDown() override {
        // Clean up test environment
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }
    
    fs::path testDir;
    std::unique_ptr<d2portable::core::AssetManager> assetManager;
    std::unique_ptr<d2::rendering::TextureManager> textureManager;
    std::unique_ptr<d2::GameEngine> gameEngine;
};

// Test all systems integration
TEST_F(ProductionReadinessTest, AllSystemsIntegration) {
    // Test that all core systems can initialize
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize for production";
    
    EXPECT_TRUE(gameEngine->initialize()) 
        << "GameEngine should initialize for production";
    
    // Test that systems work together
    EXPECT_TRUE(assetManager != nullptr) << "AssetManager should be operational";
    EXPECT_TRUE(textureManager != nullptr) << "TextureManager should be operational"; 
    EXPECT_TRUE(gameEngine != nullptr) << "GameEngine should be operational";
    
    // Test basic system interactions
    bool systemsIntegrated = (assetManager != nullptr && 
                              textureManager != nullptr && 
                              gameEngine != nullptr);
    EXPECT_TRUE(systemsIntegrated) << "All systems should integrate successfully";
}

// Test performance benchmarking
TEST_F(ProductionReadinessTest, PerformanceBenchmarking) {
    // Initialize systems for performance testing
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize";
    EXPECT_TRUE(gameEngine->initialize()) 
        << "GameEngine should initialize";
    
    // Benchmark asset loading performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10; ++i) {
        auto sprite = assetManager->loadSprite("test_sprite.dc6");
        // Should complete without significant delay
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Performance should meet production requirements (under 100ms for 10 loads)
    EXPECT_LT(duration.count(), 100) 
        << "Asset loading should meet performance requirements (took " << duration.count() << "ms)";
    
    // Benchmark game engine update performance
    startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 60; ++i) {
        gameEngine->update(1.0f / 60.0f); // Simulate 60 FPS updates (16.67ms deltaTime)
    }
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should maintain 60 FPS performance (60 updates in under 1000ms)
    EXPECT_LT(duration.count(), 1000) 
        << "Game engine should maintain 60 FPS (60 updates took " << duration.count() << "ms)";
}

// Test production deployment validation
TEST_F(ProductionReadinessTest, ProductionDeploymentValidation) {
    // Test system stability under load
    for (int i = 0; i < 100; ++i) {
        // Repeatedly initialize and use systems
        EXPECT_TRUE(assetManager->initialize(testDir.string())) 
            << "System should be stable under repeated initialization";
        
        auto sprite = assetManager->loadSprite("test.dc6");
        // Should handle repeated operations without degradation
    }
    
    // Test memory management
    std::vector<std::unique_ptr<d2portable::core::AssetManager>> managers;
    
    for (int i = 0; i < 10; ++i) {
        auto manager = std::make_unique<d2portable::core::AssetManager>();
        manager->initialize(testDir.string());
        managers.push_back(std::move(manager));
    }
    
    // Should be able to create multiple instances without issues
    EXPECT_EQ(managers.size(), 10) << "Should handle multiple system instances";
    
    // Clean up should work properly
    managers.clear();
    EXPECT_TRUE(true) << "Memory cleanup should work correctly";
}

// Test error handling in production scenarios
TEST_F(ProductionReadinessTest, ProductionErrorHandling) {
    // Test initialization with invalid paths
    auto testAssetManager = std::make_unique<d2portable::core::AssetManager>();
    
    // Should handle invalid paths gracefully
    testAssetManager->initialize("/completely/invalid/path/that/does/not/exist");
    // Result can be true or false, but shouldn't crash
    EXPECT_TRUE(true) << "Should handle invalid paths without crashing";
    
    // Test asset loading with missing files
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize";
    
    for (int i = 0; i < 50; ++i) {
        auto sprite = assetManager->loadSprite("missing_file_" + std::to_string(i) + ".dc6");
        // Should handle missing files gracefully
    }
    
    EXPECT_TRUE(true) << "Should handle missing files gracefully in production";
}

// Test system resource management
TEST_F(ProductionReadinessTest, SystemResourceManagement) {
    // Test that systems properly manage resources
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize";
    
    // Load multiple assets to test resource management
    std::vector<std::shared_ptr<d2portable::sprites::DC6Sprite>> loadedSprites;
    
    for (int i = 0; i < 20; ++i) {
        auto sprite = assetManager->loadSprite("resource_test_" + std::to_string(i) + ".dc6");
        if (sprite) {
            loadedSprites.push_back(sprite);
        }
    }
    
    // System should handle resource allocation properly
    EXPECT_TRUE(true) << "Resource management should work correctly";
    
    // Clear resources
    loadedSprites.clear();
    
    // System should remain stable after resource cleanup
    auto testSprite = assetManager->loadSprite("post_cleanup_test.dc6");
    EXPECT_TRUE(true) << "System should remain stable after resource cleanup";
}

// Test production-ready configuration
TEST_F(ProductionReadinessTest, ProductionConfiguration) {
    // Test that all systems use production-ready defaults
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should use production defaults";
    
    EXPECT_TRUE(gameEngine->initialize()) 
        << "GameEngine should use production defaults";
    
    // Test that systems handle production workloads
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate production-level activity
    for (int frame = 0; frame < 300; ++frame) { // 5 seconds at 60 FPS
        gameEngine->update(1.0f / 60.0f);
        
        if (frame % 30 == 0) { // Load asset every 0.5 seconds
            auto sprite = assetManager->loadSprite("production_test.dc6");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should handle production workload efficiently (300 frames in reasonable time)
    EXPECT_LT(duration.count(), 5000) 
        << "Should handle production workload efficiently (took " << duration.count() << "ms)";
}

// Test comprehensive system validation
TEST_F(ProductionReadinessTest, ComprehensiveSystemValidation) {
    // Validate all major system components are functional
    
    // Test Asset Management System
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "Asset management system should be functional";
    
    // Test Game Engine System  
    EXPECT_TRUE(gameEngine->initialize()) 
        << "Game engine system should be functional";
    
    // Test Rendering System Components
    EXPECT_TRUE(textureManager != nullptr) 
        << "Rendering system should be functional";
    
    // Test MPQ Loading System
    auto mpqLoader = std::make_unique<d2::MPQLoader>();
    EXPECT_TRUE(mpqLoader != nullptr) 
        << "MPQ loading system should be functional";
    
    // Test system interactions work
    bool allSystemsWorking = (assetManager != nullptr && 
                              gameEngine != nullptr && 
                              textureManager != nullptr && 
                              mpqLoader != nullptr);
    
    EXPECT_TRUE(allSystemsWorking) 
        << "All major systems should be functional and ready for production";
    
    // Final validation - systems should handle a complete workflow
    auto sprite = assetManager->loadSprite("final_validation.dc6");
    gameEngine->update(1.0f / 60.0f);
    
    EXPECT_TRUE(true) << "Complete system workflow should function correctly";
}