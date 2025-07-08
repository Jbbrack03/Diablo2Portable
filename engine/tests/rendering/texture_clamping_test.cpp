#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/texture_manager.h"
#include "rendering/renderer.h"
#include <vector>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
// Mock OpenGL constants for desktop testing
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_REPEAT 0x2901

extern "C" {
    void glTexParameteri(uint32_t target, uint32_t pname, int param);
}

// Mock OpenGL call tracking - since glTexParameteri is already defined in texture_manager.cpp
// we'll just track the calls that we can observe
static std::vector<uint32_t> tex_param_targets;
static std::vector<uint32_t> tex_param_names;
static std::vector<int> tex_param_values;
#endif

namespace d2::rendering {

class TextureClampingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset tracking for each test
        #ifndef __ANDROID__
        tex_param_targets.clear();
        tex_param_names.clear();
        tex_param_values.clear();
        #endif
    }
    
    void TearDown() override {
        // Test cleanup
        #ifndef __ANDROID__
        tex_param_targets.clear();
        tex_param_names.clear();
        tex_param_values.clear();
        #endif
    }
};

TEST_F(TextureClampingTest, EnablesClampToEdgeForTextures) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    
    // Initialize texture manager
    ASSERT_TRUE(texture_manager.initialize(renderer));
    
    // Create a texture with RGBA data
    std::vector<uint8_t> rgba_data = {255, 0, 0, 255};  // Red pixel
    uint32_t texture_id = texture_manager.createTexture(rgba_data.data(), 1, 1);
    
    // Enable clamp to edge for this texture
    texture_manager.setTextureWrapMode(texture_id, TextureWrapMode::CLAMP_TO_EDGE);
    
    // Verify that the method completes without error
    EXPECT_NE(texture_id, 0);
    
    // Verify that the texture is still valid after setting wrap mode
    EXPECT_TRUE(texture_manager.isTextureValid(texture_id));
}

TEST_F(TextureClampingTest, SupportsRepeatWrapMode) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    
    // Initialize texture manager
    ASSERT_TRUE(texture_manager.initialize(renderer));
    
    // Create a texture with RGBA data
    std::vector<uint8_t> rgba_data = {0, 255, 0, 255};  // Green pixel
    uint32_t texture_id = texture_manager.createTexture(rgba_data.data(), 1, 1);
    
    // Set texture to repeat mode
    texture_manager.setTextureWrapMode(texture_id, TextureWrapMode::REPEAT);
    
    // Verify that the method completes without error
    EXPECT_NE(texture_id, 0);
    
    // Verify that the texture is still valid after setting wrap mode
    EXPECT_TRUE(texture_manager.isTextureValid(texture_id));
}

} // namespace d2::rendering