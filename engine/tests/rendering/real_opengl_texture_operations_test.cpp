#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/texture_manager.h"
#include <vector>

namespace d2::rendering {

class RealOpenGLTextureOperationsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test RGBA data for a 2x2 texture
        test_rgba_data = {
            255, 0, 0, 255,    // Red pixel
            0, 255, 0, 255,    // Green pixel
            0, 0, 255, 255,    // Blue pixel
            255, 255, 255, 255 // White pixel
        };
    }

    std::vector<uint8_t> test_rgba_data;
};

// RED PHASE: This test MUST fail - requires real OpenGL texture operations
TEST_F(RealOpenGLTextureOperationsTest, ActuallyCallsGLGenTextures) {
    TextureManager manager;
    
    // Test that texture creation actually validates OpenGL texture limits and errors
    // Current mock implementation doesn't check for OpenGL errors or texture size limits
    
    // Test with extremely large texture dimensions that should fail with real OpenGL
    // but will pass with mock because mock glTexImage2D is a no-op
    std::vector<uint8_t> large_data(16384 * 16384 * 4, 255); // 4GB of texture data
    uint32_t huge_texture = manager.createTexture(large_data.data(), 16384, 16384);
    
    // Real OpenGL should fail to create textures that exceed GPU memory or texture size limits
    // Mock implementation will always succeed because it doesn't actually call OpenGL
    // This test will FAIL with current mock because it doesn't validate texture limits
    EXPECT_EQ(huge_texture, 0u) << "Texture creation with unrealistic size should fail with real OpenGL";
    
    // Test with normal texture that should succeed
    uint32_t normal_texture = manager.createTexture(test_rgba_data.data(), 2, 2);
    EXPECT_NE(normal_texture, 0u) << "Normal texture should be created successfully";
    
    // Test that the texture has proper dimensions stored
    EXPECT_EQ(manager.getTextureWidth(normal_texture), 2u) << "Texture width should be correct";
    EXPECT_EQ(manager.getTextureHeight(normal_texture), 2u) << "Texture height should be correct";
    
    // Test with extremely small data that doesn't match dimensions
    // This should work because TextureManager validates input parameters
    uint32_t mismatched_texture = manager.createTexture(test_rgba_data.data(), 1000, 1000);
    EXPECT_EQ(mismatched_texture, 0u) << "Texture creation with mismatched data size should fail";
}

} // namespace d2::rendering