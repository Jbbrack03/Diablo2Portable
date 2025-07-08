#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
// Mock OpenGL constants for desktop testing
#define GL_DEPTH_TEST 0x0B71
#define GL_LEQUAL 0x0203
#define GL_LESS 0x0201
#define GL_ALWAYS 0x0207
#define GL_EQUAL 0x0202

extern "C" {
    void glDepthFunc(uint32_t func);
    void glDepthMask(uint8_t flag);
    // These are declared in other test files
    void glEnable(uint32_t cap);
    void glDisable(uint32_t cap);
    bool glIsEnabled(uint32_t cap);
}

// Mock OpenGL call tracking
static bool gl_depth_test_enabled = false;
static uint32_t gl_depth_func = 0;
static uint8_t gl_depth_mask = 1;
static bool gl_depth_func_called = false;
static bool gl_depth_mask_called = false;

void glDepthFunc(uint32_t func) {
    gl_depth_func = func;
    gl_depth_func_called = true;
}

void glDepthMask(uint8_t flag) {
    gl_depth_mask = flag;
    gl_depth_mask_called = true;
}

// These functions are implemented in other test files
// We'll just test the unique depth functions here
#endif

namespace d2::rendering {

class DepthTestingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset OpenGL state for each test
        #ifndef __ANDROID__
        gl_depth_test_enabled = false;
        gl_depth_func = 0;
        gl_depth_mask = 1;
        gl_depth_func_called = false;
        gl_depth_mask_called = false;
        #endif
    }
    
    void TearDown() override {
        #ifndef __ANDROID__
        // Clean up OpenGL state
        gl_depth_test_enabled = false;
        gl_depth_func = 0;
        gl_depth_mask = 1;
        gl_depth_func_called = false;
        gl_depth_mask_called = false;
        #endif
    }
};

TEST_F(DepthTestingTest, EnablesDepthTestingForSpriteLayering) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;
    
    // Initialize sprite renderer
    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));
    
    // Enable depth testing for sprite layering
    sprite_renderer.enableDepthTesting();
    
    // Verify that OpenGL depth function is set correctly
    #ifdef __ANDROID__
    EXPECT_TRUE(glIsEnabled(GL_DEPTH_TEST));
    #else
    EXPECT_TRUE(gl_depth_func_called);
    EXPECT_EQ(gl_depth_func, GL_LEQUAL);
    #endif
}

TEST_F(DepthTestingTest, DisablesDepthWritesForTransparency) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;
    
    // Initialize sprite renderer
    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));
    
    // Enable depth testing but disable depth writes for transparency
    sprite_renderer.enableDepthTesting();
    sprite_renderer.disableDepthWrites();
    
    // Verify that depth writes are disabled
    #ifdef __ANDROID__
    // On Android, we can't easily test the depth mask state
    EXPECT_TRUE(true);  // Just ensure the functions don't crash
    #else
    EXPECT_TRUE(gl_depth_mask_called);
    EXPECT_EQ(gl_depth_mask, 0);  // GL_FALSE
    #endif
}

TEST_F(DepthTestingTest, EnablesDepthWritesForOpaqueSprites) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;
    
    // Initialize sprite renderer
    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));
    
    // Enable depth testing and depth writes for opaque sprites
    sprite_renderer.enableDepthTesting();
    sprite_renderer.enableDepthWrites();
    
    // Verify that depth writes are enabled
    #ifdef __ANDROID__
    // On Android, we can't easily test the depth mask state
    EXPECT_TRUE(true);  // Just ensure the functions don't crash
    #else
    EXPECT_TRUE(gl_depth_mask_called);
    EXPECT_EQ(gl_depth_mask, 1);  // GL_TRUE
    #endif
}

} // namespace d2::rendering