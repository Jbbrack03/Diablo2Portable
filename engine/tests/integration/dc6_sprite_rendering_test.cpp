#include <gtest/gtest.h>
#include <filesystem>
#include <vector>
#include <fstream>
#include <chrono>
#include <glm/vec2.hpp>
#include "sprites/dc6_parser.h"
#include "rendering/texture_manager.h"
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "utils/mpq_validator.h"

// Phase 39.1: Verify sprite rendering with actual DC6 files
// This test validates DC6 sprite parsing and rendering pipeline

namespace fs = std::filesystem;

class DC6SpriteRenderingTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir_ = fs::temp_directory_path() / "dc6_sprite_test";
        fs::create_directories(test_dir_);
        
        // Create test DC6 files for validation
        createTestDC6Files();
    }
    
    void TearDown() override {
        fs::remove_all(test_dir_);
    }
    
    void createTestDC6Files() {
        // Create a minimal valid DC6 file for testing
        // DC6 format: Version(6) + Flags(1) + Encoding(1) + Termination(4) + Directions(4) + FramesPerDir(4)
        createMinimalDC6(test_dir_ / "test_sprite.dc6", 1, 1); // 1 direction, 1 frame
        createMinimalDC6(test_dir_ / "character_sprite.dc6", 8, 16); // 8 directions, 16 frames
        createMinimalDC6(test_dir_ / "item_sprite.dc6", 1, 1); // Simple item sprite
    }
    
    void createMinimalDC6(const fs::path& path, uint32_t dirs, uint32_t frames) {
        std::ofstream file(path, std::ios::binary);
        
        // DC6 Header
        uint32_t version = 6;
        uint32_t flags = 0;
        uint32_t encoding = 0;
        std::vector<uint8_t> termination = {0xEE, 0xEE, 0xEE, 0xEE};
        
        file.write(reinterpret_cast<char*>(&version), 4);
        file.write(reinterpret_cast<char*>(&flags), 1);
        file.write(reinterpret_cast<char*>(&encoding), 1);
        file.write(reinterpret_cast<char*>(termination.data()), 4);
        file.write(reinterpret_cast<char*>(&dirs), 4);
        file.write(reinterpret_cast<char*>(&frames), 4);
        
        // For each direction, write frame pointers
        uint32_t frameOffset = 256; // Start frames after header
        for (uint32_t d = 0; d < dirs; d++) {
            for (uint32_t f = 0; f < frames; f++) {
                file.write(reinterpret_cast<char*>(&frameOffset), 4);
                frameOffset += 100; // Each frame is 100 bytes for simplicity
            }
        }
        
        // Write minimal frame data for each frame
        for (uint32_t d = 0; d < dirs; d++) {
            for (uint32_t f = 0; f < frames; f++) {
                // Frame header
                uint32_t flip = 0;
                uint32_t width = 32;
                uint32_t height = 32;
                int32_t offsetX = 0;
                int32_t offsetY = 0;
                uint32_t unknown = 0;
                uint32_t nextBlock = 0;
                uint32_t length = 32 * 32; // Uncompressed for simplicity
                
                file.write(reinterpret_cast<char*>(&flip), 4);
                file.write(reinterpret_cast<char*>(&width), 4);
                file.write(reinterpret_cast<char*>(&height), 4);
                file.write(reinterpret_cast<char*>(&offsetX), 4);
                file.write(reinterpret_cast<char*>(&offsetY), 4);
                file.write(reinterpret_cast<char*>(&unknown), 4);
                file.write(reinterpret_cast<char*>(&nextBlock), 4);
                file.write(reinterpret_cast<char*>(&length), 4);
                
                // Write pixel data (simple gradient)
                for (uint32_t y = 0; y < height; y++) {
                    for (uint32_t x = 0; x < width; x++) {
                        uint8_t pixel = (x + y) % 256;
                        file.write(reinterpret_cast<char*>(&pixel), 1);
                    }
                }
                
                // Pad to 100 bytes per frame
                std::vector<uint8_t> padding(100 - 32 - (width * height), 0);
                file.write(reinterpret_cast<char*>(padding.data()), padding.size());
            }
        }
    }
    
    fs::path test_dir_;
};

// Test 1: Parse DC6 files
TEST_F(DC6SpriteRenderingTest, ParseDC6Files) {
    d2portable::sprites::DC6Parser parser;
    
    // Test simple sprite
    {
        auto sprite = parser.parseFile((test_dir_ / "test_sprite.dc6").string());
        ASSERT_NE(sprite, nullptr) << "Failed to parse test_sprite.dc6";
        
        EXPECT_EQ(sprite->getDirectionCount(), 1);
        EXPECT_EQ(sprite->getFramesPerDirection(), 1);
        
        // Get frame data
        auto frame = sprite->getFrame(0, 0);
        EXPECT_EQ(frame.width, 32);
        EXPECT_EQ(frame.height, 32);
        EXPECT_EQ(frame.offsetX, 0);
        EXPECT_EQ(frame.offsetY, 0);
    }
    
    // Test character sprite
    {
        auto sprite = parser.parseFile((test_dir_ / "character_sprite.dc6").string());
        ASSERT_NE(sprite, nullptr) << "Failed to parse character_sprite.dc6";
        
        EXPECT_EQ(sprite->getDirectionCount(), 8);
        EXPECT_EQ(sprite->getFramesPerDirection(), 16);
        
        // Test frame from different directions
        for (uint32_t dir = 0; dir < 8; dir++) {
            auto frame = sprite->getFrame(dir, 0);
            EXPECT_EQ(frame.width, 32);
            EXPECT_EQ(frame.height, 32);
        }
    }
}

// Test 2: Convert DC6 to RGBA for rendering
TEST_F(DC6SpriteRenderingTest, ConvertDC6ToRGBA) {
    d2portable::sprites::DC6Parser parser;
    auto sprite = parser.parseFile((test_dir_ / "test_sprite.dc6").string());
    ASSERT_NE(sprite, nullptr);
    
    // Get default palette
    auto palette = parser.getDefaultPalette();
    EXPECT_EQ(palette.size(), 256) << "Default palette should have 256 colors";
    
    // Convert frame to RGBA
    auto rgbaData = sprite->getFrameImageWithPalette(0, 0, palette);
    EXPECT_EQ(rgbaData.size(), 32 * 32 * 4) << "RGBA data should be width*height*4 bytes";
    
    // Verify RGBA format (4 bytes per pixel)
    for (size_t i = 0; i < rgbaData.size(); i += 4) {
        // Check alpha channel is set
        EXPECT_GT(rgbaData[i + 3], 0) << "Alpha channel should be non-zero for visible pixels";
    }
}

// Test 3: Create texture from DC6 sprite
TEST_F(DC6SpriteRenderingTest, CreateTextureFromDC6) {
    d2portable::sprites::DC6Parser parser;
    auto sprite = parser.parseFile((test_dir_ / "test_sprite.dc6").string());
    ASSERT_NE(sprite, nullptr);
    
    // Get RGBA data
    auto palette = parser.getDefaultPalette();
    auto rgbaData = sprite->getFrameImageWithPalette(0, 0, palette);
    
    // Create texture
    d2::rendering::Renderer renderer;
    d2::rendering::TextureManager texManager;
    ASSERT_TRUE(texManager.initialize(renderer));
    
    auto textureId = texManager.createTexture(rgbaData.data(), 32, 32);
    
    EXPECT_GT(textureId, 0) << "Valid texture ID should be greater than 0";
    
    // Verify texture properties
    EXPECT_EQ(texManager.getTextureWidth(textureId), 32);
    EXPECT_EQ(texManager.getTextureHeight(textureId), 32);
    EXPECT_TRUE(texManager.isTextureValid(textureId));
}

// Test 4: Render DC6 sprite
TEST_F(DC6SpriteRenderingTest, RenderDC6Sprite) {
    // This test validates the full pipeline from DC6 to rendered sprite
    d2portable::sprites::DC6Parser parser;
    auto sprite = parser.parseFile((test_dir_ / "character_sprite.dc6").string());
    ASSERT_NE(sprite, nullptr);
    
    d2::rendering::Renderer mainRenderer;
    d2::rendering::TextureManager texManager;
    d2::rendering::SpriteRenderer spriteRenderer;
    
    ASSERT_TRUE(texManager.initialize(mainRenderer));
    ASSERT_TRUE(spriteRenderer.initialize(mainRenderer, texManager));
    
    // Create textures for all directions
    std::vector<uint32_t> directionTextures;
    auto palette = parser.getDefaultPalette();
    
    for (uint32_t dir = 0; dir < sprite->getDirectionCount(); dir++) {
        auto rgbaData = sprite->getFrameImageWithPalette(dir, 0, palette);
        auto textureId = texManager.createTexture(rgbaData.data(), 32, 32);
        directionTextures.push_back(textureId);
    }
    
    EXPECT_EQ(directionTextures.size(), 8) << "Should have texture for each direction";
    
    // Simulate rendering a frame
    spriteRenderer.beginFrame();
    
    // Render sprite facing different directions
    glm::vec2 positions[] = {
        {100, 100}, // North
        {150, 100}, // Northeast  
        {200, 100}, // East
        {250, 100}, // Southeast
        {100, 150}, // South
        {150, 150}, // Southwest
        {200, 150}, // West
        {250, 150}  // Northwest
    };
    
    for (uint32_t dir = 0; dir < 8; dir++) {
        spriteRenderer.drawSprite(directionTextures[dir], 
                                  positions[dir], 
                                  glm::vec2(32, 32));
    }
    
    spriteRenderer.endFrame();
    EXPECT_EQ(spriteRenderer.getSpriteCount(), 8) << "Should have drawn 8 sprites";
}

// Test 5: Performance test for DC6 rendering
TEST_F(DC6SpriteRenderingTest, DC6RenderingPerformance) {
    d2portable::sprites::DC6Parser parser;
    auto sprite = parser.parseFile((test_dir_ / "item_sprite.dc6").string());
    ASSERT_NE(sprite, nullptr);
    
    d2::rendering::Renderer mainRenderer;
    d2::rendering::TextureManager texManager;
    d2::rendering::SpriteRenderer spriteRenderer;
    
    ASSERT_TRUE(texManager.initialize(mainRenderer));
    ASSERT_TRUE(spriteRenderer.initialize(mainRenderer, texManager));
    
    // Create texture
    auto palette = parser.getDefaultPalette();
    auto rgbaData = sprite->getFrameImageWithPalette(0, 0, palette);
    auto textureId = texManager.createTexture(rgbaData.data(), 32, 32);
    
    // Measure rendering performance
    const int SPRITE_COUNT = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    spriteRenderer.beginFrame();
    
    for (int i = 0; i < SPRITE_COUNT; i++) {
        float x = (i % 40) * 25.0f;
        float y = (i / 40) * 25.0f;
        spriteRenderer.drawSprite(textureId, glm::vec2(x, y), glm::vec2(32, 32));
    }
    
    spriteRenderer.endFrame();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    EXPECT_EQ(spriteRenderer.getSpriteCount(), SPRITE_COUNT);
    
    double timePerSprite = static_cast<double>(duration) / SPRITE_COUNT;
    std::cout << "\nDC6 Rendering Performance:\n";
    std::cout << "  Sprites rendered: " << SPRITE_COUNT << "\n";
    std::cout << "  Total time: " << duration << "μs\n";
    std::cout << "  Time per sprite: " << timePerSprite << "μs\n";
    std::cout << "  Sprites per frame (16.67ms): " 
              << (16670.0 / timePerSprite) << "\n";
    
    // Performance requirements
    EXPECT_LT(timePerSprite, 100.0) << "Rendering time per sprite too high";
}

// Test 6: Test placeholder MPQ handling
TEST_F(DC6SpriteRenderingTest, HandlePlaceholderMPQs) {
    // This test documents expected behavior with placeholder MPQs
    fs::path mpq_dir = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
    
    std::cout << "\n=== Placeholder MPQ Handling ===\n";
    
    // Check known placeholder files
    std::vector<std::string> critical_mpqs = {"d2data.mpq", "d2exp.mpq"};
    
    for (const auto& mpq_name : critical_mpqs) {
        fs::path mpq_path = mpq_dir / mpq_name;
        
        if (fs::exists(mpq_path)) {
            auto validation = d2::utils::MPQValidator::validateMPQFile(mpq_path.string());
            
            std::cout << mpq_name << ": ";
            if (validation.isPlaceholder) {
                std::cout << "PLACEHOLDER (would contain DC6 sprites in real game)\n";
            } else if (validation.isValid) {
                std::cout << "VALID MPQ\n";
            } else {
                std::cout << "INVALID (" << validation.error << ")\n";
            }
        } else {
            std::cout << mpq_name << ": NOT FOUND\n";
        }
    }
    
    std::cout << "\nNote: Real d2data.mpq would contain:\n";
    std::cout << "  - Character sprites (data/global/chars/)\n";
    std::cout << "  - Monster sprites (data/global/monsters/)\n";
    std::cout << "  - Item sprites (data/global/items/)\n";
    std::cout << "  - UI elements (data/global/ui/)\n";
}