#include <gtest/gtest.h>
#include "tools/apk_packager.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace d2;

class APKPackagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directories
        tempPath = fs::temp_directory_path() / "test_apk_packager";
        fs::create_directories(tempPath);
        
        // Create test assets directory
        assetsPath = tempPath / "test_assets";
        fs::create_directories(assetsPath);
        
        // Create output directory
        outputPath = tempPath / "apk_output";
        fs::create_directories(outputPath);
    }
    
    void TearDown() override {
        fs::remove_all(tempPath);
    }
    
    void createTestFile(const fs::path& path, const std::string& content) {
        fs::create_directories(path.parent_path());
        std::ofstream file(path);
        file << content;
        file.close();
    }
    
    fs::path tempPath;
    fs::path assetsPath;
    fs::path outputPath;
};

TEST_F(APKPackagerTest, CreatePackager) {
    APKPackager packager;
    EXPECT_EQ(packager.getAssetCount(), 0);
    EXPECT_EQ(packager.getTotalSize(), 0);
}

TEST_F(APKPackagerTest, AddAsset) {
    APKPackager packager;
    
    // Create test files
    auto file1 = assetsPath / "sprites" / "player.png";
    createTestFile(file1, "fake png data");
    
    auto file2 = assetsPath / "sounds" / "music.ogg";
    createTestFile(file2, "fake ogg data for music");
    
    // Add assets
    packager.addAsset(file1.string(), "assets/sprites/player.png");
    EXPECT_EQ(packager.getAssetCount(), 1);
    
    packager.addAsset(file2.string(), "assets/sounds/music.ogg");
    EXPECT_EQ(packager.getAssetCount(), 2);
    
    // Check total size
    EXPECT_EQ(packager.getTotalSize(), 13 + 23); // "fake png data" + "fake ogg data for music"
    
    // Clear and verify
    packager.clear();
    EXPECT_EQ(packager.getAssetCount(), 0);
    EXPECT_EQ(packager.getTotalSize(), 0);
}

TEST_F(APKPackagerTest, PackageAssets) {
    APKPackager packager;
    
    // Create test files
    auto sprite = assetsPath / "sprites" / "player.png";
    createTestFile(sprite, "PNG sprite data");
    
    auto sound = assetsPath / "sounds" / "effect.ogg";
    createTestFile(sound, "OGG sound data");
    
    auto data = assetsPath / "data" / "config.json";
    createTestFile(data, "{\"version\":1}");
    
    // Add assets
    packager.addAsset(sprite.string(), "assets/sprites/player.png");
    packager.addAsset(sound.string(), "assets/sounds/effect.ogg");
    packager.addAsset(data.string(), "assets/data/config.json");
    
    // Package assets
    EXPECT_TRUE(packager.packageAssets(outputPath.string()));
    
    // Verify output structure
    EXPECT_TRUE(fs::exists(outputPath / "assets"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sprites"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sounds"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "data"));
    
    // Verify files were copied
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sprites" / "player.png"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sounds" / "effect.ogg"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "data" / "config.json"));
    
    // Verify content
    std::ifstream spriteFile(outputPath / "assets" / "sprites" / "player.png");
    std::string spriteContent((std::istreambuf_iterator<char>(spriteFile)),
                              std::istreambuf_iterator<char>());
    EXPECT_EQ(spriteContent, "PNG sprite data");
}