#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/shader_manager.h"

namespace d2::rendering {

class RealOpenGLShaderCompilationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Valid vertex shader source
        vertex_shader_source = R"(
            #version 300 es
            layout(location = 0) in vec2 a_position;
            layout(location = 1) in vec2 a_texcoord;
            uniform mat4 u_projection;
            out vec2 v_texcoord;
            void main() {
                gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
                v_texcoord = a_texcoord;
            }
        )";
        
        // Valid fragment shader source
        fragment_shader_source = R"(
            #version 300 es
            precision mediump float;
            in vec2 v_texcoord;
            uniform sampler2D u_texture;
            out vec4 fragColor;
            void main() {
                fragColor = texture(u_texture, v_texcoord);
            }
        )";
    }

    std::string vertex_shader_source;
    std::string fragment_shader_source;
};

// RED PHASE: This test MUST fail - requires real OpenGL shader compilation
TEST_F(RealOpenGLShaderCompilationTest, CompileShaderCallsGLCreateShader) {
    ShaderManager manager;
    
    // Test that compilation actually validates shader syntax correctly
    // This should fail because the shader source has intentional syntax errors
    std::string invalid_vertex_shader = R"(
        #version 300 es
        layout(location = 0) in vec2 a_position;
        uniform mat4 u_projection;
        void main() {
            gl_Position = u_projection * vec4(a_position, 0.0, 1.0)
        }
    )";
    
    // This should fail to compile and return 0 (invalid shader ID)
    uint32_t invalid_shader_id = manager.compileShader(ShaderType::VERTEX, invalid_vertex_shader);
    EXPECT_EQ(invalid_shader_id, 0u) << "Invalid shader should not compile and return 0";
    
    // Valid shader should compile successfully
    uint32_t valid_shader_id = manager.compileShader(ShaderType::VERTEX, vertex_shader_source);
    EXPECT_NE(valid_shader_id, 0u) << "Valid shader should compile successfully";
    
    // The shader manager should track actual OpenGL shader compilation status
    // This test will fail with current simulation because it doesn't actually validate GLSL
    std::string another_invalid_shader = R"(
        #version 300 es
        this is not valid GLSL at all
        random text that should never compile
    )";
    
    uint32_t another_invalid_id = manager.compileShader(ShaderType::FRAGMENT, another_invalid_shader);
    EXPECT_EQ(another_invalid_id, 0u) << "Completely invalid shader should not compile and return 0";
}

} // namespace d2::rendering