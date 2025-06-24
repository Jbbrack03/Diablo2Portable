#include <gtest/gtest.h>
#include "tools/texture_atlas_generator.h"
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;
using namespace d2;

class TextureAtlasTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directories
        inputPath = fs::temp_directory_path() / "test_atlas_input";
        outputPath = fs::temp_directory_path() / "test_atlas_output";
        
        fs::create_directories(inputPath);
        fs::create_directories(outputPath);
        
        // Create mock sprite files
        createMockSprite(inputPath / "sprite1.png", 128, 128);
        createMockSprite(inputPath / "sprite2.png", 64, 64);
        createMockSprite(inputPath / "sprite3.png", 256, 256);
    }
    
    void TearDown() override {
        fs::remove_all(inputPath);
        fs::remove_all(outputPath);
    }
    
    void createMockSprite(const fs::path& path, int width, int height) {
        // Create a simple PNG file for testing
        // For now, just create a file with expected size
        std::ofstream file(path, std::ios::binary);
        
        // Simple PNG header (this is a simplified version for testing)
        std::vector<uint8_t> header = {
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
        };
        file.write(reinterpret_cast<const char*>(header.data()), header.size());
        
        // Add some dummy data proportional to dimensions
        std::vector<uint8_t> data(width * height * 4, 0xAB);
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        
        file.close();
    }
    
    fs::path inputPath;
    fs::path outputPath;
};

TEST_F(TextureAtlasTest, GenerateAtlasFromSprites) {
    TextureAtlasGenerator generator;
    
    std::vector<std::string> sprites = {
        (inputPath / "sprite1.png").string(),
        (inputPath / "sprite2.png").string(),
        (inputPath / "sprite3.png").string()
    };
    
    auto atlas = generator.generateAtlas(sprites, 2048, 2048);
    
    EXPECT_TRUE(atlas.isValid());
    EXPECT_LE(atlas.getPageCount(), 2); // Should fit in 1-2 pages
    EXPECT_TRUE(atlas.hasSprite("sprite1.png"));
}

TEST_F(TextureAtlasTest, EfficientPacking) {
    TextureAtlasGenerator generator;
    
    // Create many small sprites that should fit on one page
    std::vector<std::string> sprites;
    for (int i = 0; i < 20; ++i) {
        auto spritePath = inputPath / ("small_sprite_" + std::to_string(i) + ".png");
        createMockSprite(spritePath, 64, 64);
        sprites.push_back(spritePath.string());
    }
    
    auto atlas = generator.generateAtlas(sprites, 512, 512);
    
    // 20 sprites of 64x64 = 20 * 4096 = 81,920 pixels
    // 512x512 = 262,144 pixels
    // Should fit on one page with good packing
    EXPECT_EQ(atlas.getPageCount(), 1);
    
    // Verify all sprites are included
    for (int i = 0; i < 20; ++i) {
        std::string spriteName = "small_sprite_" + std::to_string(i) + ".png";
        EXPECT_TRUE(atlas.hasSprite(spriteName));
    }
}