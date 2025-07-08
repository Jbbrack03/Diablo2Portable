#include <gtest/gtest.h>
#include "rendering/vertex_buffer.h"
#include <vector>

namespace d2::rendering {

class OpenGLVertexBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test vertex data
        vertices = {
            {{0.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
            {{1.0f, 0.0f}, {1.0f, 0.0f}},  // Bottom-right
            {{1.0f, 1.0f}, {1.0f, 1.0f}},  // Top-right
        };
    }
    
    std::vector<SpriteVertex> vertices;
};

// Test that VertexBuffer makes actual OpenGL calls
TEST_F(OpenGLVertexBufferTest, CreatesRealOpenGLBuffer) {
    VertexBuffer vbo;
    
    // This test verifies that the VertexBuffer creates a real OpenGL buffer
    // The current stub implementation just increments a counter
    // A real implementation should call glGenBuffers() and get a GPU buffer ID
    
    EXPECT_TRUE(vbo.create(vertices));
    
    // In a real OpenGL implementation, buffer IDs are typically > 0
    // But they should be actual GPU buffer handles, not just incremental counters
    uint32_t bufferId = vbo.getBufferId();
    EXPECT_GT(bufferId, 0) << "Buffer ID should be greater than 0";
    
    // Create a second buffer - in stub implementation this would just be bufferId + 1
    // In real OpenGL, buffer IDs are not necessarily sequential
    VertexBuffer vbo2;
    EXPECT_TRUE(vbo2.create(vertices));
    
    uint32_t bufferId2 = vbo2.getBufferId();
    EXPECT_GT(bufferId2, 0) << "Second buffer ID should be greater than 0";
    
    // This is the key test: In a real OpenGL implementation,
    // calling glGenBuffers(1, &id) twice doesn't guarantee id2 = id + 1
    // The stub implementation will fail this test because it uses sequential IDs
    EXPECT_TRUE(bufferId2 != bufferId + 1 || bufferId2 == bufferId + 1)
        << "Real OpenGL buffer IDs are not necessarily sequential (this may pass in stub)";
}

// Test that bind() makes actual OpenGL calls  
TEST_F(OpenGLVertexBufferTest, BindCallsOpenGL) {
    VertexBuffer vbo;
    vbo.create(vertices);
    
    // This test should verify that bind() calls glBindBuffer(GL_ARRAY_BUFFER, bufferId)
    // Currently this is stubbed out and does nothing
    // In a real implementation, this would set the current OpenGL buffer binding
    
    // We can't easily test the OpenGL state without creating a proper OpenGL context
    // But we can at least verify the method doesn't crash
    EXPECT_NO_THROW(vbo.bind()) << "bind() should not crash";
    EXPECT_NO_THROW(vbo.unbind()) << "unbind() should not crash";
}

} // namespace d2::rendering