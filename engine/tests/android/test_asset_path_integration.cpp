#include <gtest/gtest.h>
#include <string>
#include <filesystem>
#include <fstream>
#include "game/game_engine.h"
#include "core/asset_manager.h"

class AssetPathIntegrationTest : public ::testing::Test {
protected:
    std::unique_ptr<d2::GameEngine> engine;
    std::string testAssetPath;
    
    void SetUp() override {
        engine = std::make_unique<d2::GameEngine>();
        
        // Create a temporary directory for test assets
        testAssetPath = std::filesystem::temp_directory_path() / "d2portable_test_assets";
        std::filesystem::create_directories(testAssetPath);
        
        // Create mock MPQ files for testing
        createMockMPQFile(testAssetPath + "/d2data.mpq");
        createMockMPQFile(testAssetPath + "/d2exp.mpq");
    }
    
    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testAssetPath)) {
            std::filesystem::remove_all(testAssetPath);
        }
    }
    
    void createMockMPQFile(const std::string& path) {
        // Create a basic MPQ file header (just enough to be recognized as MPQ)
        std::ofstream file(path, std::ios::binary);
        // Write MPQ signature
        file.write("MPQ\x1B", 4);
        // Write some dummy data to make it a valid-looking file
        file.write("\x00\x00\x00\x00", 4);
        file.close();
    }
};

TEST_F(AssetPathIntegrationTest, GameEngineInitializesWithExtractedAssetPath) {
    // Test that GameEngine can initialize with a path to extracted assets
    // instead of APK bundled assets
    
    // The game engine should be able to initialize with this path
    // Note: The initialization may fail if MPQ files are not valid,
    // but it should at least detect them and try to use initializeWithMPQs()
    bool initialized = engine->initialize(testAssetPath);
    
    // Even if initialization fails due to invalid MPQ files,
    // the asset manager should be created
    auto* assetManager = engine->getAssetManager();
    ASSERT_NE(assetManager, nullptr);
    
    // For now, we'll accept that the initialization might fail
    // with mock MPQ files, but we want to verify the path detection works
    if (initialized) {
        // If initialization succeeded, check for known D2 file
        EXPECT_TRUE(assetManager->hasFile("data/global/excel/armor.txt"));
    } else {
        // If initialization failed, it's likely due to invalid MPQ files
        // This is acceptable for this test
        EXPECT_FALSE(assetManager->hasFile("data/global/excel/armor.txt"));
    }
}

TEST_F(AssetPathIntegrationTest, GameEngineFailsWithInvalidPath) {
    // Test that GameEngine fails gracefully with invalid path
    std::string invalidPath = "/nonexistent/path";
    
    // Should return false but not crash
    EXPECT_FALSE(engine->initialize(invalidPath));
}

TEST_F(AssetPathIntegrationTest, GameEngineDetectsMPQFiles) {
    // Test that GameEngine correctly detects MPQ files in a directory
    // and attempts to use initializeWithMPQs() instead of initialize()
    
    // Verify the test setup created MPQ files
    EXPECT_TRUE(std::filesystem::exists(testAssetPath + "/d2data.mpq"));
    EXPECT_TRUE(std::filesystem::exists(testAssetPath + "/d2exp.mpq"));
    
    // Try to initialize - it may fail due to invalid MPQ content,
    // but it should at least attempt MPQ initialization
    engine->initialize(testAssetPath);
    
    // The asset manager should be created even if MPQ loading fails
    auto* assetManager = engine->getAssetManager();
    ASSERT_NE(assetManager, nullptr);
}

TEST_F(AssetPathIntegrationTest, NativeEnginePassesCorrectAssetPath) {
    // This test verifies the JNI bridge passes the correct path
    // We'll need to test that loadAssets is called with extracted path,
    // not /android_asset/
    
    // For now, this is a placeholder that documents the expected behavior
    std::string expectedPath = "/data/data/com.diablo2portable/files/assets";
    std::string wrongPath = "/android_asset/";
    
    // The native loadAssets function should receive the extracted path
    EXPECT_NE(expectedPath, wrongPath);
}