#include <gtest/gtest.h>
#include "rendering/vertex_buffer.h"
#include <vector>

namespace d2::rendering {

class VertexBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a quad (two triangles)
        vertices = {
            {{0.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
            {{1.0f, 0.0f}, {1.0f, 0.0f}},  // Bottom-right
            {{1.0f, 1.0f}, {1.0f, 1.0f}},  // Top-right
            
            {{0.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
            {{1.0f, 1.0f}, {1.0f, 1.0f}},  // Top-right
            {{0.0f, 1.0f}, {0.0f, 1.0f}}   // Top-left
        };
    }
    
    std::vector<SpriteVertex> vertices;
};

// Test that VertexBuffer can create GPU buffers
TEST_F(VertexBufferTest, CreatesGPUBuffer) {
    VertexBuffer vbo;
    
    // This should create a buffer on the GPU and return true
    EXPECT_TRUE(vbo.create(vertices)) 
        << "VertexBuffer should create GPU buffer for vertex data";
    
    EXPECT_TRUE(vbo.isValid()) 
        << "Created buffer should be valid";
    
    EXPECT_EQ(vbo.getVertexCount(), 6) 
        << "Buffer should contain 6 vertices for a quad";
}

} // namespace d2::rendering