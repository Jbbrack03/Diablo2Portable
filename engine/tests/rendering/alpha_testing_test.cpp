#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/shader_manager.h"
#include <string>

namespace d2::rendering {

class AlphaTestingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
    
    void TearDown() override {
        // Test cleanup
    }
};

TEST_F(AlphaTestingTest, EnablesAlphaTestingInFragmentShader) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;
    
    // Initialize sprite renderer
    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));
    
    // Enable alpha testing for binary transparency
    sprite_renderer.enableAlphaTesting(0.5f);  // 50% alpha threshold
    
    // Verify that the sprite renderer is configured for alpha testing
    EXPECT_TRUE(sprite_renderer.isAlphaTestingEnabled());
    EXPECT_FLOAT_EQ(sprite_renderer.getAlphaTestThreshold(), 0.5f);
}

TEST_F(AlphaTestingTest, DisablesAlphaTestingInFragmentShader) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;
    
    // Initialize sprite renderer
    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));
    
    // Enable then disable alpha testing
    sprite_renderer.enableAlphaTesting(0.1f);
    sprite_renderer.disableAlphaTesting();
    
    // Verify that alpha testing is disabled
    EXPECT_FALSE(sprite_renderer.isAlphaTestingEnabled());
}

TEST_F(AlphaTestingTest, UsesAlphaTestingFragmentShader) {
    // Create necessary objects
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;
    
    // Initialize sprite renderer
    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));
    
    // Enable alpha testing
    sprite_renderer.enableAlphaTesting(0.1f);
    
    // Get the fragment shader source and verify it contains alpha testing
    std::string fragmentShaderSource = sprite_renderer.getFragmentShaderSource();
    
    // Check that the fragment shader contains alpha testing code
    EXPECT_NE(fragmentShaderSource.find("discard"), std::string::npos);
    EXPECT_NE(fragmentShaderSource.find("u_alphaThreshold"), std::string::npos);
}

} // namespace d2::rendering