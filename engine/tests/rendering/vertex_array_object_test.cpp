#include <gtest/gtest.h>
#include "rendering/vertex_array_object.h"
#include "rendering/vertex_buffer.h"

using namespace d2::rendering;

class VertexArrayObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test environment
    }
};

// Test 1: VAO creation and OpenGL resource management
TEST_F(VertexArrayObjectTest, CreateVAOGeneratesOpenGLResource) {
    VertexArrayObject vao;
    
    // VAO should start uninitialized
    EXPECT_FALSE(vao.isValid());
    EXPECT_EQ(0, vao.getVAOId());
    
    // Create the VAO
    EXPECT_TRUE(vao.create());
    
    // VAO should now have a valid OpenGL ID
    EXPECT_TRUE(vao.isValid());
    EXPECT_NE(0, vao.getVAOId());
}