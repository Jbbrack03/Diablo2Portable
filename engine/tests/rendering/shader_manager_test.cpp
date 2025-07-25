#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/shader_manager.h"

namespace d2::rendering {

class ShaderManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Basic vertex shader for sprite rendering
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

    void TearDown() override {
        // Test cleanup
    }

    std::string vertex_shader_source;
    std::string fragment_shader_source;
};

// RED PHASE: This test MUST fail - testing shader compilation
TEST_F(ShaderManagerTest, CompileVertexShader) {
    ShaderManager manager;
    
    uint32_t shader_id = manager.compileShader(ShaderType::VERTEX, vertex_shader_source);
    
    EXPECT_NE(shader_id, 0u);
    EXPECT_TRUE(manager.isShaderValid(shader_id));
}

// RED PHASE: This test MUST fail - testing shader program linking
TEST_F(ShaderManagerTest, CreateShaderProgram) {
    ShaderManager manager;
    
    // Compile both vertex and fragment shaders
    uint32_t vertex_shader = manager.compileShader(ShaderType::VERTEX, vertex_shader_source);
    uint32_t fragment_shader = manager.compileShader(ShaderType::FRAGMENT, fragment_shader_source);
    
    ASSERT_NE(vertex_shader, 0u);
    ASSERT_NE(fragment_shader, 0u);
    
    // Link shaders into a complete program
    uint32_t program_id = manager.createProgram(vertex_shader, fragment_shader);
    
    EXPECT_NE(program_id, 0u) << "Shader program should be created successfully";
    EXPECT_TRUE(manager.isProgramValid(program_id)) << "Created program should be valid";
}

// RED PHASE: This test MUST fail - testing OpenGL shader deletion
TEST_F(ShaderManagerTest, DeleteShaderAfterProgram) {
    ShaderManager manager;
    
    // Compile both vertex and fragment shaders
    uint32_t vertex_shader = manager.compileShader(ShaderType::VERTEX, vertex_shader_source);
    uint32_t fragment_shader = manager.compileShader(ShaderType::FRAGMENT, fragment_shader_source);
    
    ASSERT_NE(vertex_shader, 0u);
    ASSERT_NE(fragment_shader, 0u);
    
    // Create program
    uint32_t program_id = manager.createProgram(vertex_shader, fragment_shader);
    ASSERT_NE(program_id, 0u);
    
    // Delete individual shaders (they should be detached from program)
    manager.deleteShader(vertex_shader);
    manager.deleteShader(fragment_shader);
    
    // Program should still be valid even after deleting individual shaders
    EXPECT_TRUE(manager.isProgramValid(program_id)) << "Program should remain valid after deleting shaders";
    
    // Individual shaders should no longer be valid
    EXPECT_FALSE(manager.isShaderValid(vertex_shader)) << "Deleted vertex shader should not be valid";
    EXPECT_FALSE(manager.isShaderValid(fragment_shader)) << "Deleted fragment shader should not be valid";
}

} // namespace d2::rendering