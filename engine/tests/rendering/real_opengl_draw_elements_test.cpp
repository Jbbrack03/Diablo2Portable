#include <gtest/gtest.h>
#include <cstdint>

namespace d2::rendering {

// OpenGL constants
#define GL_TRIANGLES 0x0004
#define GL_UNSIGNED_SHORT 0x1403

// External functions from mock_opengl.cpp for draw command tracking
extern "C" {
    struct DrawElementsCall {
        uint32_t mode;
        int count;
        uint32_t type;
        uintptr_t indices;
    };
    
    void resetDrawCommandTracking();
    bool wasDrawElementsCalled();
    
    // Function to test - will fail initially
    void glDrawElements(uint32_t mode, int count, uint32_t type, const void* indices);
}

class RealOpenGLDrawElementsTest : public ::testing::Test {
protected:
    void SetUp() override {
        resetDrawCommandTracking();
    }
};

// Test that glDrawElements exists and can be called
TEST_F(RealOpenGLDrawElementsTest, DrawElementsFunctionExists) {
    // Create simple index data for a quad (2 triangles)
    uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };
    
    // This should compile and link if glDrawElements is implemented
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    
    // Verify the function was tracked
    EXPECT_TRUE(wasDrawElementsCalled()) 
        << "glDrawElements should be tracked when called";
}

} // namespace d2::rendering