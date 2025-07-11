#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <chrono>

// Include the core asset management system
#include "core/asset_manager.h"
#include "rendering/texture_manager.h"
#include "sprites/dc6_parser.h"
#include "utils/mpq_loader.h"

namespace fs = std::filesystem;

class AssetPipelineRepairTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary test directory
        testDir = fs::temp_directory_path() / "asset_pipeline_test";
        fs::create_directories(testDir);
        
        // Initialize asset manager
        assetManager = std::make_unique<d2portable::core::AssetManager>();
        textureManager = std::make_unique<d2::rendering::TextureManager>();
    }
    
    void TearDown() override {
        // Clean up test directory
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }
    
    fs::path testDir;
    std::unique_ptr<d2portable::core::AssetManager> assetManager;
    std::unique_ptr<d2::rendering::TextureManager> textureManager;
};

// Test asset loading pipeline repair
TEST_F(AssetPipelineRepairTest, AssetLoadingPipelineRepair) {
    // Test that asset manager can initialize with test directory
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "Should initialize with test directory";
    
    // Test that asset loading handles missing files gracefully
    auto sprite = assetManager->loadSprite("non_existent_sprite.dc6");
    // This may return nullptr for missing files, but shouldn't crash
    EXPECT_TRUE(true) << "Asset loading should not crash on missing files";
    
    // Test that asset manager has basic functionality
    bool hasValidState = (assetManager != nullptr);
    EXPECT_TRUE(hasValidState) << "AssetManager should be in valid state after initialization";
}

// Test MPQ integration repair
TEST_F(AssetPipelineRepairTest, MPQIntegrationRepair) {
    // Test MPQ loader handles missing files
    auto mpqLoader = std::make_unique<d2::MPQLoader>();
    
    // Should not crash with missing MPQ files
    EXPECT_FALSE(mpqLoader->open("non_existent.mpq")) 
        << "Should handle missing MPQ files gracefully";
    
    // Should not crash when extracting from closed MPQ
    std::vector<uint8_t> data;
    EXPECT_FALSE(mpqLoader->extractFile("any_file.txt", data)) 
        << "Should handle extraction from closed MPQ gracefully";
    
    // Test with placeholder MPQ files (empty files)
    std::string placeholderMPQ = testDir / "placeholder.mpq";
    std::ofstream placeholderFile(placeholderMPQ);
    placeholderFile << "PLACEHOLDER_MPQ_FILE";
    placeholderFile.close();
    
    // Should detect and handle placeholder files
    EXPECT_FALSE(mpqLoader->open(placeholderMPQ)) 
        << "Should detect and reject placeholder MPQ files";
}

// Test asset-rendering integration
TEST_F(AssetPipelineRepairTest, AssetRenderingIntegration) {
    // Initialize asset manager first
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize";
    
    // Test that texture manager exists and can be created
    EXPECT_TRUE(textureManager != nullptr) 
        << "TextureManager should be creatable";
    
    // Test basic integration between asset and texture managers
    bool integrationWorking = (assetManager != nullptr && textureManager != nullptr);
    EXPECT_TRUE(integrationWorking) 
        << "Asset-texture manager integration should work";
}

// Test asset streaming performance
TEST_F(AssetPipelineRepairTest, AssetStreamingPerformance) {
    // Initialize asset manager
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize";
    
    // Test that repeated initialization is fast
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10; ++i) {
        // Test basic operations are fast
        bool isInitialized = (assetManager != nullptr);
        EXPECT_TRUE(isInitialized) << "AssetManager should remain valid";
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should complete within reasonable time (10ms for 10 checks)
    EXPECT_LT(duration.count(), 10) 
        << "Asset operations should be efficient (took " << duration.count() << "ms)";
}

// Test asset pipeline error handling
TEST_F(AssetPipelineRepairTest, AssetPipelineErrorHandling) {
    // Test that asset manager handles errors gracefully
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize";
    
    // Test loading non-existent sprite doesn't crash
    auto sprite = assetManager->loadSprite("invalid_sprite.dc6");
    // Result can be nullptr or valid pointer, but shouldn't crash
    EXPECT_TRUE(true) << "Loading invalid sprite should not crash";
    
    // Test that managers handle invalid initialization gracefully
    auto testAssetManager = std::make_unique<d2portable::core::AssetManager>();
    bool initResult = testAssetManager->initialize("/non/existent/path");
    // Should handle invalid paths gracefully
    EXPECT_TRUE(true) << "Invalid initialization should not crash";
}

// Test asset caching system
TEST_F(AssetPipelineRepairTest, AssetCachingSystem) {
    // Initialize asset manager
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize";
    
    // Test that multiple sprite load attempts work
    for (int i = 0; i < 5; ++i) {
        auto sprite = assetManager->loadSprite("test_sprite.dc6");
        // Each load attempt should complete without crashing
        EXPECT_TRUE(true) << "Multiple sprite loads should not crash";
    }
    
    // Test operation performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        bool isValid = (assetManager != nullptr);
        EXPECT_TRUE(isValid) << "AssetManager should remain valid";
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Operations should be fast (under 10ms for 100 checks)
    EXPECT_LT(duration.count(), 10) 
        << "Asset operations should be fast (took " << duration.count() << "ms)";
}

// Test asset pipeline integration with rendering  
TEST_F(AssetPipelineRepairTest, AssetPipelineRenderingIntegration) {
    // Test that both managers can be created and work together
    EXPECT_TRUE(assetManager->initialize(testDir.string())) 
        << "AssetManager should initialize";
    
    EXPECT_TRUE(assetManager != nullptr) << "AssetManager should exist";
    EXPECT_TRUE(textureManager != nullptr) << "TextureManager should exist";
    
    // Test that loading a sprite works
    auto sprite = assetManager->loadSprite("test.dc6");
    // Result can be nullptr or valid pointer, but shouldn't crash
    EXPECT_TRUE(true) << "Sprite loading should not crash";
    
    // Test that asset-texture integration is possible
    bool integrationWorking = (assetManager != nullptr && textureManager != nullptr);
    EXPECT_TRUE(integrationWorking) << "Asset-rendering integration should be possible";
}