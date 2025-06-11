#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/egl_context.h"

namespace d2::rendering {

class EGLContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// RED PHASE: This test MUST fail - testing EGL context initialization
TEST_F(EGLContextTest, InitializeContext) {
    EGLContext context;
    
    EXPECT_TRUE(context.initialize());
    EXPECT_TRUE(context.isValid());
    EXPECT_EQ(context.getVersion(), "OpenGL ES 3.0");
}

} // namespace d2::rendering