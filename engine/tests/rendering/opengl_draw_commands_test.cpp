#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include "rendering/vertex_buffer.h"
#include "rendering/vertex_array_object.h"
#include "rendering/shader_manager.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

using namespace d2::rendering;

class OpenGLDrawCommandsTest : public ::testing::Test {
protected:
    void SetUp() override {
        renderer_ = std::make_unique<Renderer>();
        texture_manager_ = std::make_unique<TextureManager>();
        sprite_renderer_ = std::make_unique<SpriteRenderer>();
        sprite_renderer_->initialize(*renderer_, *texture_manager_);
    }
    
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<TextureManager> texture_manager_;
    std::unique_ptr<SpriteRenderer> sprite_renderer_;
};

// Test 1: SpriteRenderer creates and uses VAO for sprite rendering
TEST_F(OpenGLDrawCommandsTest, SpriteRendererUsesVAO) {
    // Get the VAO from sprite renderer (need to add this method)
    uint32_t vao_id = sprite_renderer_->getVAOId();
    EXPECT_NE(0, vao_id); // Should have created a VAO
}

// Test 2: SpriteRenderer creates vertex buffer for sprite batching
TEST_F(OpenGLDrawCommandsTest, SpriteRendererCreatesVertexBuffer) {
    // Get the vertex buffer from sprite renderer
    uint32_t vbo_id = sprite_renderer_->getVertexBufferId();
    EXPECT_NE(0, vbo_id); // Should have created a vertex buffer
}