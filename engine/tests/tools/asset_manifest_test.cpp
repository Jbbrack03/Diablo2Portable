#include <gtest/gtest.h>
#include "tools/asset_manifest.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;
using namespace d2;

class AssetManifestTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directory
        tempPath = fs::temp_directory_path() / "test_manifest";
        fs::create_directories(tempPath);
    }
    
    void TearDown() override {
        fs::remove_all(tempPath);
    }
    
    fs::path tempPath;
};

TEST_F(AssetManifestTest, GenerateManifest) {
    AssetManifest manifest;
    
    manifest.addAsset("sprites/player.png", 1024, "abc123");
    manifest.addAsset("sounds/music.ogg", 2048, "def456");
    
    auto manifestPath = tempPath / "manifest.json";
    manifest.save(manifestPath.string());
    
    AssetManifest loaded;
    loaded.load(manifestPath.string());
    
    EXPECT_EQ(loaded.getAssetCount(), 2);
    EXPECT_TRUE(loaded.hasAsset("sprites/player.png"));
}

TEST_F(AssetManifestTest, GetAssetInfo) {
    AssetManifest manifest;
    
    // Add some test assets
    manifest.addAsset("sprites/player.png", 1024, "checksum1");
    manifest.addAsset("sounds/music.ogg", 2048, "checksum2");
    manifest.addAsset("data/config.json", 512, "checksum3");
    
    // Test getting existing asset info
    const auto* info = manifest.getAssetInfo("sprites/player.png");
    ASSERT_NE(info, nullptr);
    EXPECT_EQ(info->path, "sprites/player.png");
    EXPECT_EQ(info->size, 1024);
    EXPECT_EQ(info->checksum, "checksum1");
    EXPECT_EQ(info->type, "sprite");  // Should be detected from .png extension
    
    // Test getting another asset
    const auto* soundInfo = manifest.getAssetInfo("sounds/music.ogg");
    ASSERT_NE(soundInfo, nullptr);
    EXPECT_EQ(soundInfo->path, "sounds/music.ogg");
    EXPECT_EQ(soundInfo->size, 2048);
    EXPECT_EQ(soundInfo->checksum, "checksum2");
    EXPECT_EQ(soundInfo->type, "sound");  // Should be detected from .ogg extension
    
    // Test getting non-existent asset
    const auto* notFound = manifest.getAssetInfo("does/not/exist.txt");
    EXPECT_EQ(notFound, nullptr);
}

TEST_F(AssetManifestTest, GetAssetsByType) {
    AssetManifest manifest;
    
    // Add various types of assets
    manifest.addAsset("sprites/player.png", 1024, "check1");
    manifest.addAsset("sprites/monster.png", 2048, "check2");
    manifest.addAsset("sprites/item.webp", 512, "check3");
    manifest.addAsset("sounds/music.ogg", 4096, "check4");
    manifest.addAsset("sounds/effect.wav", 1024, "check5");
    manifest.addAsset("data/config.json", 256, "check6");
    manifest.addAsset("data/strings.txt", 128, "check7");
    manifest.addAsset("misc/readme.md", 64, "check8");  // Unknown type
    
    // Get all sprites
    auto sprites = manifest.getAssetsByType("sprite");
    EXPECT_EQ(sprites.size(), 3);
    EXPECT_TRUE(std::find(sprites.begin(), sprites.end(), "sprites/player.png") != sprites.end());
    EXPECT_TRUE(std::find(sprites.begin(), sprites.end(), "sprites/monster.png") != sprites.end());
    EXPECT_TRUE(std::find(sprites.begin(), sprites.end(), "sprites/item.webp") != sprites.end());
    
    // Get all sounds
    auto sounds = manifest.getAssetsByType("sound");
    EXPECT_EQ(sounds.size(), 2);
    EXPECT_TRUE(std::find(sounds.begin(), sounds.end(), "sounds/music.ogg") != sounds.end());
    EXPECT_TRUE(std::find(sounds.begin(), sounds.end(), "sounds/effect.wav") != sounds.end());
    
    // Get all data files
    auto data = manifest.getAssetsByType("data");
    EXPECT_EQ(data.size(), 2);
    EXPECT_TRUE(std::find(data.begin(), data.end(), "data/config.json") != data.end());
    EXPECT_TRUE(std::find(data.begin(), data.end(), "data/strings.txt") != data.end());
    
    // Get unknown type
    auto unknown = manifest.getAssetsByType("unknown");
    EXPECT_EQ(unknown.size(), 1);
    EXPECT_TRUE(std::find(unknown.begin(), unknown.end(), "misc/readme.md") != unknown.end());
    
    // Get non-existent type
    auto empty = manifest.getAssetsByType("nonexistent");
    EXPECT_EQ(empty.size(), 0);
}

TEST_F(AssetManifestTest, GetTotalSize) {
    AssetManifest manifest;
    
    // Empty manifest should have 0 size
    EXPECT_EQ(manifest.getTotalSize(), 0);
    
    // Add some assets
    manifest.addAsset("file1.png", 1024, "check1");
    EXPECT_EQ(manifest.getTotalSize(), 1024);
    
    manifest.addAsset("file2.ogg", 2048, "check2");
    EXPECT_EQ(manifest.getTotalSize(), 3072);  // 1024 + 2048
    
    manifest.addAsset("file3.json", 512, "check3");
    EXPECT_EQ(manifest.getTotalSize(), 3584);  // 1024 + 2048 + 512
    
    // Add a large file
    manifest.addAsset("bigfile.dat", 1048576, "check4");  // 1MB
    EXPECT_EQ(manifest.getTotalSize(), 1052160);  // 3584 + 1048576
    
    // Clear and verify
    manifest.clear();
    EXPECT_EQ(manifest.getTotalSize(), 0);
}

TEST_F(AssetManifestTest, VersionManagement) {
    AssetManifest manifest;
    
    // Default version should be 1
    EXPECT_EQ(manifest.getVersion(), 1);
    
    // Set a new version
    manifest.setVersion(5);
    EXPECT_EQ(manifest.getVersion(), 5);
    
    // Add some assets
    manifest.addAsset("file1.png", 1024, "check1");
    manifest.addAsset("file2.ogg", 2048, "check2");
    
    // Save with version
    auto manifestPath = tempPath / "versioned_manifest.json";
    manifest.save(manifestPath.string());
    
    // Load and verify version is preserved
    AssetManifest loaded;
    loaded.load(manifestPath.string());
    EXPECT_EQ(loaded.getVersion(), 5);
    EXPECT_EQ(loaded.getAssetCount(), 2);
}

TEST_F(AssetManifestTest, ErrorHandling) {
    AssetManifest manifest;
    
    // Test loading non-existent file
    EXPECT_FALSE(manifest.load("non/existent/file.json"));
    EXPECT_EQ(manifest.getAssetCount(), 0); // Should remain empty
    
    // Test saving to invalid path
    manifest.addAsset("test.png", 1024, "check1");
    EXPECT_FALSE(manifest.save("/invalid/path/that/does/not/exist/manifest.json"));
    
    // Create a valid manifest file
    auto validPath = tempPath / "valid_manifest.json";
    EXPECT_TRUE(manifest.save(validPath.string()));
    
    // Verify we can load it
    AssetManifest loaded;
    EXPECT_TRUE(loaded.load(validPath.string()));
    EXPECT_EQ(loaded.getAssetCount(), 1);
}

TEST_F(AssetManifestTest, ChecksumPreservation) {
    AssetManifest manifest;
    
    // Add assets with specific checksums
    manifest.addAsset("file1.png", 1024, "sha256:abcdef1234567890");
    manifest.addAsset("file2.ogg", 2048, "md5:0987654321fedcba");
    manifest.addAsset("file3.json", 512, "crc32:deadbeef");
    
    // Save manifest
    auto manifestPath = tempPath / "checksum_manifest.json";
    EXPECT_TRUE(manifest.save(manifestPath.string()));
    
    // Load and verify checksums are preserved
    AssetManifest loaded;
    EXPECT_TRUE(loaded.load(manifestPath.string()));
    
    const auto* info1 = loaded.getAssetInfo("file1.png");
    ASSERT_NE(info1, nullptr);
    EXPECT_EQ(info1->checksum, "sha256:abcdef1234567890");
    
    const auto* info2 = loaded.getAssetInfo("file2.ogg");
    ASSERT_NE(info2, nullptr);
    EXPECT_EQ(info2->checksum, "md5:0987654321fedcba");
    
    const auto* info3 = loaded.getAssetInfo("file3.json");
    ASSERT_NE(info3, nullptr);
    EXPECT_EQ(info3->checksum, "crc32:deadbeef");
}