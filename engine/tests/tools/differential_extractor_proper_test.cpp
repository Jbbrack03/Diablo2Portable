#include <gtest/gtest.h>
#include "tools/differential_extractor.h"
#include "tools/asset_manifest.h"
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;
using namespace d2;

class DifferentialExtractorProperTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directories for testing
        testD2Path = fs::temp_directory_path() / "test_d2_diff_proper";
        extractedPath = fs::temp_directory_path() / "test_extracted_diff_proper";
        
        fs::create_directories(testD2Path);
        fs::create_directories(extractedPath);
        
        // Create a more realistic test structure with extracted assets
        createExtractedAssets();
    }
    
    void TearDown() override {
        fs::remove_all(testD2Path);
        fs::remove_all(extractedPath);
    }
    
    void createExtractedAssets() {
        // Create some sample extracted assets
        fs::create_directories(extractedPath / "sprites" / "characters");
        fs::create_directories(extractedPath / "sprites" / "monsters");
        fs::create_directories(extractedPath / "sounds" / "effects");
        
        // Create test asset files
        createTestAsset(extractedPath / "sprites" / "characters" / "barbarian.dc6", "barb_data_v1");
        createTestAsset(extractedPath / "sprites" / "monsters" / "skeleton.dc6", "skel_data_v1");
        createTestAsset(extractedPath / "sounds" / "effects" / "sword_hit.wav", "sword_sound_v1");
    }
    
    void createTestAsset(const fs::path& path, const std::string& content) {
        std::ofstream file(path, std::ios::binary);
        file << content;
        file.close();
    }
    
    void modifyTestAsset(const fs::path& path, const std::string& newContent) {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        file << newContent;
        file.close();
    }
    
    fs::path testD2Path;
    fs::path extractedPath;
};

TEST_F(DifferentialExtractorProperTest, DetectChangedExtractedAssets) {
    DifferentialExtractor extractor;
    
    // Generate initial manifest from extracted assets
    auto manifest1 = extractor.generateManifest(extractedPath.string());
    
    // Verify manifest has the correct assets
    EXPECT_TRUE(manifest1->hasAsset("sprites/characters/barbarian.dc6"));
    EXPECT_TRUE(manifest1->hasAsset("sprites/monsters/skeleton.dc6"));
    EXPECT_TRUE(manifest1->hasAsset("sounds/effects/sword_hit.wav"));
    
    // Modify one of the extracted assets
    modifyTestAsset(extractedPath / "sprites" / "characters" / "barbarian.dc6", "barb_data_v2_modified");
    
    // Add a new asset
    createTestAsset(extractedPath / "sprites" / "characters" / "sorceress.dc6", "sorc_data_v1");
    
    // Delete an asset
    fs::remove(extractedPath / "sounds" / "effects" / "sword_hit.wav");
    
    // Detect changes
    auto changes = extractor.detectChangesInExtractedAssets(extractedPath.string(), *manifest1);
    
    // Verify detected changes
    EXPECT_EQ(changes.modifiedFiles.size(), 1);
    EXPECT_TRUE(changes.hasFile("sprites/characters/barbarian.dc6"));
    EXPECT_EQ(changes.changeType("sprites/characters/barbarian.dc6"), ChangeType::MODIFIED);
    
    EXPECT_EQ(changes.addedFiles.size(), 1);
    EXPECT_TRUE(changes.hasFile("sprites/characters/sorceress.dc6"));
    EXPECT_EQ(changes.changeType("sprites/characters/sorceress.dc6"), ChangeType::ADDED);
    
    EXPECT_EQ(changes.deletedFiles.size(), 1);
    EXPECT_TRUE(changes.hasFile("sounds/effects/sword_hit.wav"));
    EXPECT_EQ(changes.changeType("sounds/effects/sword_hit.wav"), ChangeType::DELETED);
}