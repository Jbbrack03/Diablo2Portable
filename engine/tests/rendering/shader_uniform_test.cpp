#include <gtest/gtest.h>
#include "rendering/shader_manager.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace d2::rendering;

class ShaderUniformTest : public ::testing::Test {
protected:
    void SetUp() override {
        shader_manager_ = std::make_unique<ShaderManager>();
        
        // Create a simple shader program for testing
        const std::string vertex_source = R"(
            #version 300 es
            uniform mat4 u_projection;
            void main() {
                gl_Position = u_projection * vec4(0.0, 0.0, 0.0, 1.0);
            }
        )";
        
        const std::string fragment_source = R"(
            #version 300 es
            precision mediump float;
            uniform vec4 u_color;
            out vec4 fragColor;
            void main() {
                fragColor = u_color;
            }
        )";
        
        uint32_t vs = shader_manager_->compileShader(ShaderType::VERTEX, vertex_source);
        uint32_t fs = shader_manager_->compileShader(ShaderType::FRAGMENT, fragment_source);
        program_id_ = shader_manager_->createProgram(vs, fs);
        shader_manager_->deleteShader(vs);
        shader_manager_->deleteShader(fs);
    }
    
    std::unique_ptr<ShaderManager> shader_manager_;
    uint32_t program_id_ = 0;
};

// Test 1: Getting uniform locations from shader program
TEST_F(ShaderUniformTest, GetUniformLocation) {
    ASSERT_NE(0, program_id_);
    
    // Get location of projection matrix uniform
    int projection_location = shader_manager_->getUniformLocation(program_id_, "u_projection");
    EXPECT_GE(projection_location, 0);  // Valid locations are >= 0
    
    // Get location of color uniform
    int color_location = shader_manager_->getUniformLocation(program_id_, "u_color");
    EXPECT_GE(color_location, 0);
    
    // Non-existent uniform should return -1
    int invalid_location = shader_manager_->getUniformLocation(program_id_, "u_doesnt_exist");
    EXPECT_EQ(-1, invalid_location);
}