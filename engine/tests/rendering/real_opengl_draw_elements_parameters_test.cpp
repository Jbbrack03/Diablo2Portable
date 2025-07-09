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
    size_t getDrawElementsCallCount();
    const DrawElementsCall* getDrawElementsCalls();
    
    void glDrawElements(uint32_t mode, int count, uint32_t type, const void* indices);
}

class RealOpenGLDrawElementsParametersTest : public ::testing::Test {
protected:
    void SetUp() override {
        resetDrawCommandTracking();
    }
};

// Test that glDrawElements parameters are tracked correctly
TEST_F(RealOpenGLDrawElementsParametersTest, TracksDrawElementsParameters) {
    // Create index data for a quad
    uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };
    
    // Call glDrawElements
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    
    // Verify call count
    EXPECT_EQ(getDrawElementsCallCount(), 1u)
        << "Should have exactly one glDrawElements call";
    
    // Verify the parameters were tracked correctly
    const DrawElementsCall* calls = getDrawElementsCalls();
    ASSERT_NE(calls, nullptr) << "Should have draw calls tracked";
    
    const auto& call = calls[0];
    EXPECT_EQ(call.mode, GL_TRIANGLES)
        << "Should track GL_TRIANGLES mode";
    EXPECT_EQ(call.count, 6)
        << "Should track count of 6 indices";
    EXPECT_EQ(call.type, GL_UNSIGNED_SHORT)
        << "Should track GL_UNSIGNED_SHORT type";
    EXPECT_EQ(call.indices, reinterpret_cast<uintptr_t>(indices))
        << "Should track indices pointer";
}

} // namespace d2::rendering