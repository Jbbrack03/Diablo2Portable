#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
// Mock OpenGL constants for desktop testing
#define GL_BLEND 0x0BE2
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803

extern "C" {
    void glEnable(uint32_t cap);
    void glDisable(uint32_t cap);
    void glBlendFunc(uint32_t src, uint32_t dst);
    bool glIsEnabled(uint32_t cap);
}

// Mock OpenGL call tracking
static bool gl_blend_enabled = false;
static uint32_t gl_blend_src = 0;
static uint32_t gl_blend_dst = 0;
static bool gl_blend_func_called = false;

void glEnable(uint32_t cap) {
    if (cap == GL_BLEND) {
        gl_blend_enabled = true;
    }
}

void glDisable(uint32_t cap) {
    if (cap == GL_BLEND) {
        gl_blend_enabled = false;
    }
}

void glBlendFunc(uint32_t src, uint32_t dst) {
    gl_blend_src = src;
    gl_blend_dst = dst;
    gl_blend_func_called = true;
}

bool glIsEnabled(uint32_t cap) {
    if (cap == GL_BLEND) {
        return gl_blend_enabled;
    }
    return false;
}
#endif

namespace d2::rendering {

class AlphaBlendingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset OpenGL state for each test
        #ifndef __ANDROID__
        gl_blend_enabled = false;
        gl_blend_src = 0;
        gl_blend_dst = 0;
        gl_blend_func_called = false;
        #endif
    }
    
    void TearDown() override {
        #ifndef __ANDROID__
        // Clean up OpenGL state
        gl_blend_enabled = false;
        gl_blend_src = 0;
        gl_blend_dst = 0;
        gl_blend_func_called = false;
        #endif
    }
};

TEST_F(AlphaBlendingTest, EnablesAlphaBlendingForTransparency) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;
    
    // Initialize sprite renderer
    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));
    
    // Enable alpha blending for transparency
    sprite_renderer.enableAlphaBlending();
    
    // Verify that OpenGL blending is enabled
    #ifdef __ANDROID__
    EXPECT_TRUE(glIsEnabled(GL_BLEND));
    #else
    EXPECT_TRUE(gl_blend_enabled);
    EXPECT_TRUE(gl_blend_func_called);
    EXPECT_EQ(gl_blend_src, GL_SRC_ALPHA);
    EXPECT_EQ(gl_blend_dst, GL_ONE_MINUS_SRC_ALPHA);
    #endif
}

} // namespace d2::rendering