#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/renderer.h"
#include "rendering/egl_context.h"

namespace d2::rendering {

class RendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EGLContext>();
        context->initialize();
    }

    void TearDown() override {
        context.reset();
    }

    std::unique_ptr<EGLContext> context;
};

// RED PHASE: This test MUST fail - testing OpenGL ES renderer initialization
TEST_F(RendererTest, InitializeRenderer) {
    Renderer renderer;
    
    EXPECT_TRUE(renderer.initialize(*context));
    EXPECT_TRUE(renderer.isInitialized());
    EXPECT_EQ(renderer.getOpenGLVersion(), "OpenGL ES 3.0");
}

} // namespace d2::rendering