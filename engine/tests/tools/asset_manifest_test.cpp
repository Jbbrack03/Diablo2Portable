#include <gtest/gtest.h>
#include "tools/asset_manifest.h"
#include <filesystem>
#include <fstream>

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