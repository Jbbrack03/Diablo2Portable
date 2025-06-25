#include <gtest/gtest.h>
#include "tools/apk_packager.h"
#include "tools/asset_manifest.h"
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

TEST_F(APKPackagerTest, CompressionSupport) {
    APKPackager packager;
    
    // Create a larger test file to see compression effect
    auto largeFile = assetsPath / "data" / "large_data.txt";
    std::string largeContent(10000, 'A'); // 10KB of 'A' characters
    createTestFile(largeFile, largeContent);
    
    packager.addAsset(largeFile.string(), "assets/data/large_data.txt");
    
    // Package with compression enabled
    APKPackager::PackageOptions options;
    options.compressAssets = true;
    options.compressionLevel = 9; // Maximum compression
    
    EXPECT_TRUE(packager.packageAssets(outputPath.string(), options));
    
    // Verify file exists but is compressed (should be smaller than original)
    auto compressedFile = outputPath / "assets" / "data" / "large_data.txt.gz";
    EXPECT_TRUE(fs::exists(compressedFile));
    
    // Compressed file should be significantly smaller than 10KB
    size_t compressedSize = fs::file_size(compressedFile);
    EXPECT_LT(compressedSize, 1000); // Should compress to less than 1KB
}

TEST_F(APKPackagerTest, AddAssetDirectory) {
    APKPackager packager;
    
    // Create test directory with multiple files
    auto testDir = assetsPath / "test_sprites";
    fs::create_directories(testDir);
    
    createTestFile(testDir / "player1.png", "PNG data 1");
    createTestFile(testDir / "player2.png", "PNG data 2");
    createTestFile(testDir / "player3.png", "PNG data 3");
    
    // Create subdirectory
    auto subDir = testDir / "animations";
    fs::create_directories(subDir);
    createTestFile(subDir / "walk.png", "Walk animation");
    createTestFile(subDir / "run.png", "Run animation");
    
    // Add entire directory
    packager.addAssetDirectory(testDir.string(), "assets/sprites");
    
    // Should have added all 5 files
    EXPECT_EQ(packager.getAssetCount(), 5);
    
    // Package and verify
    EXPECT_TRUE(packager.packageAssets(outputPath.string()));
    
    // Verify all files exist in output
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sprites" / "player1.png"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sprites" / "player2.png"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sprites" / "player3.png"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sprites" / "animations" / "walk.png"));
    EXPECT_TRUE(fs::exists(outputPath / "assets" / "sprites" / "animations" / "run.png"));
}

TEST_F(APKPackagerTest, GenerateAssetIndex) {
    APKPackager packager;
    
    // Create test files
    auto sprite = assetsPath / "sprites" / "player.png";
    createTestFile(sprite, "PNG sprite data");
    
    auto sound = assetsPath / "sounds" / "effect.ogg";
    createTestFile(sound, "OGG sound data");
    
    // Add assets
    packager.addAsset(sprite.string(), "assets/sprites/player.png");
    packager.addAsset(sound.string(), "assets/sounds/effect.ogg");
    
    // Package with index generation enabled
    APKPackager::PackageOptions options;
    options.generateIndex = true;
    
    EXPECT_TRUE(packager.packageAssets(outputPath.string(), options));
    
    // Verify index file was created
    auto indexFile = outputPath / "assets" / "index.json";
    EXPECT_TRUE(fs::exists(indexFile));
    
    // Verify index contains asset entries
    std::ifstream file(indexFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_NE(content.find("player.png"), std::string::npos);
    EXPECT_NE(content.find("effect.ogg"), std::string::npos);
}

TEST_F(APKPackagerTest, ManifestIntegration) {
    APKPackager packager;
    auto manifest = std::make_shared<AssetManifest>();
    
    // Create test files
    auto sprite = assetsPath / "sprites" / "player.png";
    createTestFile(sprite, "PNG sprite data");
    
    // Set manifest
    packager.setManifest(manifest);
    
    // Add asset
    packager.addAsset(sprite.string(), "assets/sprites/player.png");
    
    // Package assets
    EXPECT_TRUE(packager.packageAssets(outputPath.string()));
    
    // Verify manifest was updated
    auto info = manifest->getAssetInfo("assets/sprites/player.png");
    EXPECT_NE(info, nullptr);
    EXPECT_EQ(info->size, 15); // "PNG sprite data" = 15 bytes
    EXPECT_EQ(info->type, "image/png");
    EXPECT_FALSE(info->checksum.empty());
}