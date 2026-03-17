#include <gtest/gtest.h>
#include "rendering/render_context.h"
#include "rendering/mock_render_backend.h"
#include <cstdint>

namespace d2::rendering {

constexpr GLenum GL_TRIANGLES_TEST = 0x0004;
constexpr GLenum GL_UNSIGNED_SHORT_TEST = 0x1403;

class RealOpenGLDrawElementsTest : public ::testing::Test {
protected:
    void SetUp() override {
        backend_ = dynamic_cast<MockRenderBackend*>(RenderContext::getBackend());
        ASSERT_NE(backend_, nullptr);
        backend_->resetDrawCommandTracking();
    }

    MockRenderBackend* backend_ = nullptr;
};

// Test that drawElements exists and can be called
TEST_F(RealOpenGLDrawElementsTest, DrawElementsFunctionExists) {
    // Create simple index data for a quad (2 triangles)
    uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };

    // Call through backend
    backend_->drawElements(GL_TRIANGLES_TEST, 6, GL_UNSIGNED_SHORT_TEST, indices);

    // Verify the function was tracked
    EXPECT_TRUE(backend_->wasDrawElementsCalled())
        << "drawElements should be tracked when called";
}

} // namespace d2::rendering
