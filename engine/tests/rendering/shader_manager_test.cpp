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

} // namespace d2::rendering