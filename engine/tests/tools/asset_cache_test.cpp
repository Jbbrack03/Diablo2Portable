#include <gtest/gtest.h>
#include "tools/asset_cache.h"
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;
using namespace d2;

class AssetCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directories for testing
        testPath = fs::temp_directory_path() / "test_asset_cache";
        fs::create_directories(testPath);
        
        // Create test assets
        createTestAsset("sprite1.dc6", 1024 * 100);  // 100KB
        createTestAsset("sprite2.dc6", 1024 * 200);  // 200KB
        createTestAsset("sprite3.dc6", 1024 * 150);  // 150KB
        createTestAsset("sound1.wav", 1024 * 500);   // 500KB
    }
    
    void TearDown() override {
        fs::remove_all(testPath);
    }
    
    void createTestAsset(const std::string& filename, size_t size) {
        fs::path filepath = testPath / filename;
        std::ofstream file(filepath, std::ios::binary);
        std::vector<char> data(size, 'A');
        file.write(data.data(), data.size());
    }
    
    fs::path testPath;
};

TEST_F(AssetCacheTest, CreateCacheWithMemoryLimit) {
    // Create cache with 1MB limit
    AssetCache cache(1024 * 1024);
    
    EXPECT_EQ(cache.getMaxMemory(), 1024 * 1024);
    EXPECT_EQ(cache.getCurrentMemory(), 0);
    EXPECT_EQ(cache.getCacheHits(), 0);
    EXPECT_EQ(cache.getCacheMisses(), 0);
}

TEST_F(AssetCacheTest, LoadAssetIntoCache) {
    AssetCache cache(1024 * 1024); // 1MB limit
    
    // Load sprite1.dc6 (100KB)
    fs::path assetPath = testPath / "sprite1.dc6";
    auto data = cache.loadAsset(assetPath.string());
    
    EXPECT_TRUE(data != nullptr);
    EXPECT_EQ(data->size(), 1024 * 100); // 100KB
    EXPECT_EQ(cache.getCurrentMemory(), 1024 * 100);
    EXPECT_EQ(cache.getCacheMisses(), 1); // First load is a miss
    
    // Load same asset again - should be a cache hit
    auto data2 = cache.loadAsset(assetPath.string());
    EXPECT_TRUE(data2 != nullptr);
    EXPECT_EQ(data2->size(), 1024 * 100);
    EXPECT_EQ(cache.getCurrentMemory(), 1024 * 100); // Still only 100KB
    EXPECT_EQ(cache.getCacheHits(), 1);
    EXPECT_EQ(cache.getCacheMisses(), 1);
}

TEST_F(AssetCacheTest, LRUEvictionWhenFull) {
    AssetCache cache(1024 * 400); // 400KB limit - can fit 2 assets max
    
    // Load three assets to trigger eviction
    auto data1 = cache.loadAsset((testPath / "sprite1.dc6").string()); // 100KB
    auto data2 = cache.loadAsset((testPath / "sprite2.dc6").string()); // 200KB
    auto data3 = cache.loadAsset((testPath / "sprite3.dc6").string()); // 150KB
    
    // Total would be 450KB, but limit is 400KB
    // sprite1 should be evicted (oldest)
    EXPECT_EQ(cache.getCurrentMemory(), 1024 * 350); // 200KB + 150KB
    EXPECT_EQ(cache.getCacheMisses(), 3);
    
    // Access sprite1 again - should be a miss (was evicted)
    auto data1_again = cache.loadAsset((testPath / "sprite1.dc6").string());
    EXPECT_EQ(cache.getCacheMisses(), 4);
    EXPECT_EQ(cache.getCacheHits(), 0);
    
    // sprite2 should now be evicted (200KB) to make room for sprite1 (100KB)
    EXPECT_EQ(cache.getCurrentMemory(), 1024 * 250); // 150KB + 100KB
}