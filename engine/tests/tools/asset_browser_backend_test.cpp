#include <gtest/gtest.h>
#include "tools/asset_browser_backend.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

namespace d2 {
namespace {

class AssetBrowserBackendTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory structure
        testDir = std::filesystem::temp_directory_path() / "asset_browser_test";
        std::filesystem::create_directories(testDir / "characters" / "barbarian");
        std::filesystem::create_directories(testDir / "monsters" / "zombie");
        std::filesystem::create_directories(testDir / "items" / "weapons");
        std::filesystem::create_directories(testDir / "ui" / "panels");
        
        // Create dummy test files
        createTestFile(testDir / "characters" / "barbarian" / "walk.dc6", 1024);
        createTestFile(testDir / "monsters" / "zombie" / "walk.dc6", 2048);
        createTestFile(testDir / "items" / "weapons" / "sword.dc6", 512);
        createTestFile(testDir / "ui" / "panels" / "inventory.dc6", 4096);
    }
    
    void TearDown() override {
        std::filesystem::remove_all(testDir);
    }
    
    void createTestFile(const std::filesystem::path& path, size_t size) {
        std::ofstream file(path, std::ios::binary);
        std::vector<char> data(size, 'A');
        file.write(data.data(), data.size());
    }
    
    std::filesystem::path testDir;
};

// Test 1: Load asset metadata - This test MUST fail first
TEST_F(AssetBrowserBackendTest, LoadAssetMetadata) {
    AssetBrowserBackend backend;
    backend.initialize(testDir.string());
    
    auto metadata = backend.getAssetMetadata("ui/panels/inventory.dc6");
    
    EXPECT_FALSE(metadata.filename.empty());
    EXPECT_GT(metadata.fileSize, 0);
    EXPECT_GT(metadata.frameCount, 0);
    EXPECT_FALSE(metadata.category.empty());
}

// Test 2: Generate thumbnails - This test MUST fail first
TEST_F(AssetBrowserBackendTest, GenerateThumbnails) {
    AssetBrowserBackend backend;
    backend.initialize(testDir.string());
    
    auto thumbnail = backend.generateThumbnail(
        "characters/barbarian/walk.dc6",
        ThumbnailSize::MEDIUM
    );
    
    EXPECT_TRUE(thumbnail.isValid());
    EXPECT_EQ(thumbnail.width, 128);
    EXPECT_EQ(thumbnail.height, 128);
    EXPECT_TRUE(thumbnail.data.size() > 0);
}

// Test 3: Get asset categories - This test MUST fail first
TEST_F(AssetBrowserBackendTest, GetAssetCategories) {
    AssetBrowserBackend backend;
    backend.initialize(testDir.string());
    
    auto categories = backend.getAssetCategories();
    
    EXPECT_EQ(categories.size(), 4);
    EXPECT_TRUE(std::find(categories.begin(), categories.end(), "characters") != categories.end());
    EXPECT_TRUE(std::find(categories.begin(), categories.end(), "monsters") != categories.end());
    EXPECT_TRUE(std::find(categories.begin(), categories.end(), "items") != categories.end());
    EXPECT_TRUE(std::find(categories.begin(), categories.end(), "ui") != categories.end());
}

// Test 4: Search assets - This test MUST fail first
TEST_F(AssetBrowserBackendTest, SearchAssets) {
    AssetBrowserBackend backend;
    backend.initialize(testDir.string());
    
    // Search for "walk" should find walk animations
    auto results = backend.searchAssets("walk");
    
    EXPECT_EQ(results.size(), 2);
    
    // Should find both barbarian and zombie walk animations
    bool foundBarbarian = false;
    bool foundZombie = false;
    for (const auto& result : results) {
        if (result.fullPath.find("barbarian") != std::string::npos) {
            foundBarbarian = true;
        }
        if (result.fullPath.find("zombie") != std::string::npos) {
            foundZombie = true;
        }
    }
    
    EXPECT_TRUE(foundBarbarian);
    EXPECT_TRUE(foundZombie);
}

} // namespace
} // namespace d2