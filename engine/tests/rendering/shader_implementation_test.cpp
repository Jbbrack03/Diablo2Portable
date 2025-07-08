#include <gtest/gtest.h>
#include "rendering/shader_manager.h"
#include "rendering/renderer.h"
#include "rendering/egl_context.h"

namespace d2::rendering {

class ShaderImplementationTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EGLContext>();
        context->initialize();
        
        renderer = std::make_unique<Renderer>();
        renderer->initialize(*context);
        
        shaderManager = std::make_unique<ShaderManager>();
    }

    std::unique_ptr<EGLContext> context;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<ShaderManager> shaderManager;
};

// Test that ShaderManager actually validates GLSL syntax
TEST_F(ShaderImplementationTest, RejectsInvalidGLSL) {
    // Define an invalid shader with syntax errors
    const std::string invalidShaderSource = R"(
        #version 300 es
        layout(location = 0) in vec2 position;
        
        void main() {
            // Missing semicolon and invalid syntax
            gl_Position = vec4(position, 0.0, 1.0) INVALID SYNTAX HERE
        }
    )";
    
    // This should fail to compile and return 0
    uint32_t shaderId = shaderManager->compileShader(
        ShaderType::VERTEX,
        invalidShaderSource
    );
    
    EXPECT_EQ(shaderId, 0) << "Invalid shader should fail to compile and return 0";
}

} // namespace d2::rendering