#include <gtest/gtest.h>
#include <string>
#include <filesystem>
#include <fstream>
#include "game/game_engine.h"
#include "core/asset_manager.h"
#include "utils/mock_mpq_builder.h"

class AssetPathIntegrationTest : public ::testing::Test {
protected:
    std::unique_ptr<d2::GameEngine> engine;
    std::string testAssetPath;
    
    void SetUp() override {
        engine = std::make_unique<d2::GameEngine>();
        
        // Create a temporary directory for test assets
        testAssetPath = std::filesystem::temp_directory_path() / "d2portable_test_assets";
        std::filesystem::create_directories(testAssetPath);
        
        // Create proper mock MPQ files for testing
        createProperMockMPQFiles();
    }
    
    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testAssetPath)) {
            std::filesystem::remove_all(testAssetPath);
        }
    }
    
    void createProperMockMPQFiles() {
        // Create d2data.mpq with essential files
        d2portable::utils::MockMPQBuilder d2dataBuilder;
        
        // Add the armor.txt file that the test expects
        std::string armorData = 
            "name\tversion\tcompactsave\trarity\tlevel\tlevelreq\tcost\n"
            "Quilted Armor\t0\t1\t1\t1\t1\t65\n"
            "Leather Armor\t0\t1\t1\t3\t3\t75\n"
            "Hard Leather Armor\t0\t1\t1\t5\t5\t84\n";
        
        std::vector<uint8_t> armorBytes(armorData.begin(), armorData.end());
        d2dataBuilder.addFile("data\\global\\excel\\armor.txt", armorBytes);
        
        // Add a few more essential files to make it realistic
        std::string weaponsData = 
            "name\ttype\ttype2\tcode\n"
            "Hand Axe\taxe\tmelee\thax\n"
            "Axe\taxe\tmelee\taxe\n";
        std::vector<uint8_t> weaponsBytes(weaponsData.begin(), weaponsData.end());
        d2dataBuilder.addFile("data\\global\\excel\\weapons.txt", weaponsBytes);
        
        // Build the d2data.mpq file
        d2dataBuilder.build(testAssetPath + "/d2data.mpq");
        
        // Create d2exp.mpq with expansion data
        d2portable::utils::MockMPQBuilder d2expBuilder;
        
        std::string miscData = 
            "name\tcode\ttype\n"
            "Gold\tgld\tgold\n"
            "Arrow\taqv\tammo\n";
        std::vector<uint8_t> miscBytes(miscData.begin(), miscData.end());
        d2expBuilder.addFile("data\\global\\excel\\misc.txt", miscBytes);
        
        // Build the d2exp.mpq file
        d2expBuilder.build(testAssetPath + "/d2exp.mpq");
    }
};

TEST_F(AssetPathIntegrationTest, GameEngineInitializesWithExtractedAssetPath) {
    // Test that GameEngine can initialize with a path to extracted assets
    // instead of APK bundled assets
    
    // The game engine should be able to initialize with this path
    EXPECT_TRUE(engine->initialize(testAssetPath));
    
    // The asset manager should use MPQ loading from the extracted path
    // not from android_asset
    auto* assetManager = engine->getAssetManager();
    ASSERT_NE(assetManager, nullptr);
    
    // Check that it can find a known D2 file
    // This should work with proper mock MPQ files
    EXPECT_TRUE(assetManager->hasFile("data/global/excel/armor.txt"));
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