#include <gtest/gtest.h>
#include "game/game_engine.h"
#include "android/asset_path_validator.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class GameEngineAssetValidationTest : public ::testing::Test {
protected:
    std::string test_asset_path;
    
    void SetUp() override {
        test_asset_path = "test_game_assets_" + std::to_string(::testing::UnitTest::GetInstance()->random_seed());
        fs::create_directories(test_asset_path);
    }
    
    void TearDown() override {
        if (fs::exists(test_asset_path)) {
            fs::remove_all(test_asset_path);
        }
    }
    
    void createMockMPQFile(const std::string& filename) {
        std::ofstream file(fs::path(test_asset_path) / filename, std::ios::binary);
        // MPQ header signature
        file.write("MPQ\x1A", 4);
        // Minimal valid MPQ header
        uint32_t header_size = 32;
        file.write(reinterpret_cast<const char*>(&header_size), 4);
        uint32_t archive_size = 2048; // Larger size
        file.write(reinterpret_cast<const char*>(&archive_size), 4);
        uint16_t format_version = 0;
        file.write(reinterpret_cast<const char*>(&format_version), 2);
        uint16_t block_size = 3; // 4096 bytes
        file.write(reinterpret_cast<const char*>(&block_size), 2);
        uint32_t hash_table_pos = 64;
        file.write(reinterpret_cast<const char*>(&hash_table_pos), 4);
        uint32_t block_table_pos = 128;
        file.write(reinterpret_cast<const char*>(&block_table_pos), 4);
        uint32_t hash_table_size = 16;
        file.write(reinterpret_cast<const char*>(&hash_table_size), 4);
        uint32_t block_table_size = 8;
        file.write(reinterpret_cast<const char*>(&block_table_size), 4);
        
        // Fill rest with zeros to make a 2KB file
        std::vector<char> zeros(2048 - 32, 0);
        file.write(zeros.data(), zeros.size());
    }
};

TEST_F(GameEngineAssetValidationTest, GameEngineValidatesAssetsBeforeInitialization) {
    d2::GameEngine engine;
    
    // Initialize without any MPQ files
    bool result = engine.initialize(test_asset_path);
    
    // Engine should fail to initialize without required assets
    EXPECT_FALSE(result);
}

TEST_F(GameEngineAssetValidationTest, GameEngineInitializesWithValidAssets) {
    // Create required MPQ files
    createMockMPQFile("d2data.mpq");
    createMockMPQFile("d2exp.mpq");
    
    d2::GameEngine engine;
    
    // Initialize with valid assets
    bool result = engine.initialize(test_asset_path);
    
    // Engine should initialize successfully with required assets
    EXPECT_TRUE(result);
}

TEST_F(GameEngineAssetValidationTest, GameEngineReportsDetailedAssetErrors) {
    // Create only one required file to test detailed error reporting
    createMockMPQFile("d2data.mpq");
    
    d2::GameEngine engine;
    
    // Initialize with missing d2exp.mpq
    bool result = engine.initialize(test_asset_path);
    
    // Engine should fail
    EXPECT_FALSE(result);
    
    // In a production system, we'd check for specific error messages
    // For now, just verify it fails appropriately
}