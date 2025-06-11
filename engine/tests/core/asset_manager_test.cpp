#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/asset_manager.h"
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

using namespace d2portable::core;
using namespace testing;

class AssetManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory structure
        test_dir = std::filesystem::temp_directory_path() / "d2portable_asset_test";
        std::filesystem::create_directories(test_dir);
        
        // Create mock Diablo II directory structure
        data_dir = test_dir / "data";
        global_dir = data_dir / "global";
        ui_dir = global_dir / "ui" / "panel";
        excel_dir = global_dir / "excel";
        
        std::filesystem::create_directories(ui_dir);
        std::filesystem::create_directories(excel_dir);
        
        // Create test files
        createTestDC6File();
        createTestDataFile();
    }

    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all(test_dir);
    }
    
    void createTestDC6File() {
        test_dc6_path = ui_dir / "invchar6.dc6";
        std::ofstream file(test_dc6_path, std::ios::binary);
        
        // Create a minimal valid DC6 file
        uint32_t version = 6;
        uint32_t flags = 0;
        uint32_t encoding = 0;
        uint32_t termination = 0xEEEEEEEE;
        uint32_t directions = 1;
        uint32_t frames_per_dir = 1;
        
        file.write(reinterpret_cast<const char*>(&version), 4);
        file.write(reinterpret_cast<const char*>(&flags), 4);
        file.write(reinterpret_cast<const char*>(&encoding), 4);
        file.write(reinterpret_cast<const char*>(&termination), 4);
        file.write(reinterpret_cast<const char*>(&directions), 4);
        file.write(reinterpret_cast<const char*>(&frames_per_dir), 4);
        
        // Frame pointer
        uint32_t frame_pointer = 256;
        file.write(reinterpret_cast<const char*>(&frame_pointer), 4);
        
        // Frame data
        file.seekp(frame_pointer);
        uint32_t flip = 0, width = 16, height = 16;
        int32_t offset_x = -8, offset_y = -8;
        uint32_t allocsize = 0, next_block = 0, length = 256;
        
        file.write(reinterpret_cast<const char*>(&flip), 4);
        file.write(reinterpret_cast<const char*>(&width), 4);
        file.write(reinterpret_cast<const char*>(&height), 4);
        file.write(reinterpret_cast<const char*>(&offset_x), 4);
        file.write(reinterpret_cast<const char*>(&offset_y), 4);
        file.write(reinterpret_cast<const char*>(&allocsize), 4);
        file.write(reinterpret_cast<const char*>(&next_block), 4);
        file.write(reinterpret_cast<const char*>(&length), 4);
        
        // Pixel data (16x16 = 256 bytes)
        for (int i = 0; i < 256; i++) {
            uint8_t pixel = static_cast<uint8_t>(i % 256);
            file.write(reinterpret_cast<const char*>(&pixel), 1);
        }
    }
    
    void createTestDataFile() {
        test_data_path = excel_dir / "skills.txt";
        std::ofstream file(test_data_path);
        file << "skill_data\tcol1\tcol2\n";
        file << "value1\tdata1\tdata2\n";
        file << "value2\tdata3\tdata4\n";
        file.close();
    }

    std::filesystem::path test_dir;
    std::filesystem::path data_dir;
    std::filesystem::path global_dir;
    std::filesystem::path ui_dir;
    std::filesystem::path excel_dir;
    std::filesystem::path test_dc6_path;
    std::filesystem::path test_data_path;
};

// Test: Initialize asset manager with valid data directory
TEST_F(AssetManagerTest, InitializeWithValidDirectory) {
    AssetManager manager;
    
    EXPECT_FALSE(manager.isInitialized());
    EXPECT_TRUE(manager.initialize(test_dir.string()));
    EXPECT_TRUE(manager.isInitialized());
}

// Test: Initialize with non-existent directory
TEST_F(AssetManagerTest, InitializeWithInvalidDirectory) {
    AssetManager manager;
    
    EXPECT_FALSE(manager.initialize("/path/to/nonexistent/directory"));
    EXPECT_FALSE(manager.isInitialized());
    EXPECT_FALSE(manager.getLastError().empty());
}

// Test: Check if files exist in asset system
TEST_F(AssetManagerTest, CheckFileExists) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    EXPECT_TRUE(manager.hasFile("data/global/ui/panel/invchar6.dc6"));
    EXPECT_TRUE(manager.hasFile("data/global/excel/skills.txt"));
    EXPECT_FALSE(manager.hasFile("data/global/ui/panel/nonexistent.dc6"));
}

// Test: Load DC6 sprite synchronously
TEST_F(AssetManagerTest, LoadSpriteSync) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    auto sprite = manager.loadSprite("data/global/ui/panel/invchar6.dc6");
    
    ASSERT_TRUE(sprite != nullptr);
    EXPECT_EQ(sprite->getDirectionCount(), 1);
    EXPECT_EQ(sprite->getFramesPerDirection(), 1);
    
    // Check that the sprite is cached
    EXPECT_EQ(manager.getCachedAssetCount(), 1);
    EXPECT_GT(manager.getCacheMemoryUsage(), 0);
}

// Test: Cache functionality - loading same sprite twice returns same pointer
TEST_F(AssetManagerTest, SpriteCaching) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    auto sprite1 = manager.loadSprite("data/global/ui/panel/invchar6.dc6");
    auto sprite2 = manager.loadSprite("data/global/ui/panel/invchar6.dc6");
    
    ASSERT_TRUE(sprite1 != nullptr);
    ASSERT_TRUE(sprite2 != nullptr);
    EXPECT_EQ(sprite1.get(), sprite2.get()); // Same pointer = cached
    EXPECT_EQ(manager.getCachedAssetCount(), 1);
}

// Test: Load raw file data
TEST_F(AssetManagerTest, LoadFileData) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    auto data = manager.loadFileData("data/global/excel/skills.txt");
    
    EXPECT_FALSE(data.empty());
    
    std::string content(data.begin(), data.end());
    EXPECT_THAT(content, HasSubstr("skill_data"));
    EXPECT_THAT(content, HasSubstr("value1"));
}

// Test: Async sprite loading
TEST_F(AssetManagerTest, LoadSpriteAsync) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    auto future_sprite = manager.loadSpriteAsync("data/global/ui/panel/invchar6.dc6");
    
    // Should complete within reasonable time
    auto status = future_sprite.wait_for(std::chrono::seconds(5));
    EXPECT_EQ(status, std::future_status::ready);
    
    auto sprite = future_sprite.get();
    ASSERT_TRUE(sprite != nullptr);
    EXPECT_EQ(sprite->getDirectionCount(), 1);
}

// Test: Asset information retrieval
TEST_F(AssetManagerTest, GetAssetInfo) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    // Load an asset first
    auto sprite = manager.loadSprite("data/global/ui/panel/invchar6.dc6");
    ASSERT_TRUE(sprite != nullptr);
    
    auto info = manager.getAssetInfo("data/global/ui/panel/invchar6.dc6");
    EXPECT_EQ(info.status, AssetStatus::LOADED);
    EXPECT_GT(info.memory_size, 0);
    EXPECT_EQ(info.filepath, "data/global/ui/panel/invchar6.dc6");
}

// Test: Cache memory management
TEST_F(AssetManagerTest, CacheMemoryManagement) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    // Set a small cache limit
    manager.setMaxCacheSize(1024); // 1KB limit
    
    auto sprite = manager.loadSprite("data/global/ui/panel/invchar6.dc6");
    ASSERT_TRUE(sprite != nullptr);
    
    // Cache should respect the limit
    EXPECT_LE(manager.getCacheMemoryUsage(), 1024);
}

// Test: Clear cache functionality
TEST_F(AssetManagerTest, ClearCache) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    // Load some assets
    auto sprite = manager.loadSprite("data/global/ui/panel/invchar6.dc6");
    ASSERT_TRUE(sprite != nullptr);
    
    EXPECT_GT(manager.getCachedAssetCount(), 0);
    EXPECT_GT(manager.getCacheMemoryUsage(), 0);
    
    manager.clearCache();
    
    EXPECT_EQ(manager.getCachedAssetCount(), 0);
    EXPECT_EQ(manager.getCacheMemoryUsage(), 0);
}

// Test: Handle loading non-existent sprite
TEST_F(AssetManagerTest, LoadNonExistentSprite) {
    AssetManager manager;
    ASSERT_TRUE(manager.initialize(test_dir.string()));
    
    auto sprite = manager.loadSprite("data/global/ui/panel/nonexistent.dc6");
    
    EXPECT_TRUE(sprite == nullptr);
    EXPECT_FALSE(manager.getLastError().empty());
}