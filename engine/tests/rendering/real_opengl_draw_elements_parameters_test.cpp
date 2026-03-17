#include <gtest/gtest.h>
#include "rendering/render_context.h"
#include "rendering/mock_render_backend.h"
#include <cstdint>

namespace d2::rendering {

constexpr GLenum GL_TRIANGLES_TEST = 0x0004;
constexpr GLenum GL_UNSIGNED_SHORT_TEST = 0x1403;

class RealOpenGLDrawElementsParametersTest : public ::testing::Test {
protected:
    void SetUp() override {
        backend_ = dynamic_cast<MockRenderBackend*>(RenderContext::getBackend());
        ASSERT_NE(backend_, nullptr);
        backend_->resetDrawCommandTracking();
    }

    MockRenderBackend* backend_ = nullptr;
};

// Test that drawElements parameters are tracked correctly
TEST_F(RealOpenGLDrawElementsParametersTest, TracksDrawElementsParameters) {
    // Create index data for a quad
    uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };

    // Call drawElements
    backend_->drawElements(GL_TRIANGLES_TEST, 6, GL_UNSIGNED_SHORT_TEST, indices);

    // Verify call count
    EXPECT_EQ(backend_->getDrawElementsCallCount(), 1u)
        << "Should have exactly one drawElements call";

    // Verify the parameters were tracked correctly
    const auto& calls = backend_->getDrawElementsCalls();
    ASSERT_FALSE(calls.empty()) << "Should have draw calls tracked";

    const auto& call = calls[0];
    EXPECT_EQ(call.mode, GL_TRIANGLES_TEST)
        << "Should track GL_TRIANGLES mode";
    EXPECT_EQ(call.count, 6)
        << "Should track count of 6 indices";
    EXPECT_EQ(call.type, GL_UNSIGNED_SHORT_TEST)
        << "Should track GL_UNSIGNED_SHORT type";
    EXPECT_EQ(call.indices, reinterpret_cast<uintptr_t>(indices))
        << "Should track indices pointer";
}

} // namespace d2::rendering
