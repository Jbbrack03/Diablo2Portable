#include <gtest/gtest.h>
#include "rendering/vertex_buffer.h"
#include <vector>

namespace d2::rendering {

class OpenGLCallsRequiredTest : public ::testing::Test {
protected:
    void SetUp() override {
        vertices = {
            {{0.0f, 0.0f}, {0.0f, 0.0f}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}},
        };
    }
    
    std::vector<SpriteVertex> vertices;
};

// Test that fails with stub implementation but passes with real OpenGL
TEST_F(OpenGLCallsRequiredTest, VertexBufferUsesRealOpenGL) {
    VertexBuffer vbo;
    
    // The stub implementation uses a static counter starting at 1
    // Real OpenGL buffer IDs are not predictable counters
    EXPECT_TRUE(vbo.create(vertices));
    
    // This test will fail with the current stub implementation
    // because it uses predictable sequential IDs (1, 2, 3, ...)
    // Real OpenGL may return any valid buffer ID
    uint32_t bufferId = vbo.getBufferId();
    
    // Create multiple buffers to detect the pattern
    VertexBuffer vbo2, vbo3;
    EXPECT_TRUE(vbo2.create(vertices));
    EXPECT_TRUE(vbo3.create(vertices));
    
    uint32_t bufferId2 = vbo2.getBufferId();
    uint32_t bufferId3 = vbo3.getBufferId();
    
    // In the stub implementation: bufferId=1, bufferId2=2, bufferId3=3
    // This test will fail because it detects the predictable pattern
    bool isSequential = (bufferId == 1 && bufferId2 == 2 && bufferId3 == 3);
    
    EXPECT_FALSE(isSequential) 
        << "Buffer IDs should not be predictable sequential numbers (1,2,3...) - this indicates stub implementation";
}

} // namespace d2::rendering