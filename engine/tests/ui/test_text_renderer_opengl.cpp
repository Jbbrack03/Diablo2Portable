#include <gtest/gtest.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ui/text_renderer.h"
#include "ui/font.h"

namespace d2 {

class TextRendererOpenGLTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test font with real character data
        font_ = std::make_shared<Font>("TestFont", 16);
        createTestFontFile();
        font_->loadFromFile("test_font.fnt");
    }
    
    void TearDown() override {
        std::remove("test_font.fnt");
        std::remove("test_font.png");
    }
    
    std::shared_ptr<Font> font_;
    
private:
    void createTestFontFile() {
        std::ofstream fnt("test_font.fnt");
        fnt << "info face=\"TestFont\" size=16 bold=0 italic=0\n";
        fnt << "common lineHeight=20 base=16 scaleW=256 scaleH=256 pages=1\n";
        fnt << "page id=0 file=\"test_font.png\"\n";
        fnt << "chars count=5\n";
        fnt << "char id=72  x=0   y=0   width=10 height=14 xoffset=1 yoffset=2 xadvance=12 page=0\n";  // 'H'
        fnt << "char id=101 x=10  y=0   width=8  height=10 xoffset=1 yoffset=6 xadvance=9  page=0\n";  // 'e'
        fnt << "char id=108 x=18  y=0   width=4  height=14 xoffset=1 yoffset=2 xadvance=5  page=0\n";  // 'l'
        fnt << "char id=111 x=22  y=0   width=8  height=10 xoffset=1 yoffset=6 xadvance=9  page=0\n";  // 'o'
        fnt << "char id=32  x=30  y=0   width=1  height=1  xoffset=0 yoffset=0 xadvance=4  page=0\n";  // ' '
        fnt.close();
        
        // Create dummy texture file
        std::ofstream png("test_font.png", std::ios::binary);
        unsigned char pngSig[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        png.write(reinterpret_cast<char*>(pngSig), 8);
        png.close();
    }
};

TEST_F(TextRendererOpenGLTest, CompileTextShaders) {
    TextRenderer textRenderer;
    
    // This test should fail with inline implementation
    // Real implementation needs to compile vertex and fragment shaders
    EXPECT_TRUE(textRenderer.initializeWithShaders());
    
    // Should have valid compiled shader program
    EXPECT_GT(textRenderer.getShaderProgram(), 0);
    
    // Should have shader uniforms for projection matrix, texture, and color
    EXPECT_GE(textRenderer.getUniformLocation("projection"), 0);
    EXPECT_GE(textRenderer.getUniformLocation("textTexture"), 0);
    EXPECT_GE(textRenderer.getUniformLocation("textColor"), 0);
}

TEST_F(TextRendererOpenGLTest, CreateVertexBufferWithAttributes) {
    TextRenderer textRenderer;
    textRenderer.initializeWithShaders();
    
    // Should create proper vertex buffer with position and texture coordinates
    EXPECT_GT(textRenderer.getVertexBuffer(), 0);
    EXPECT_GT(textRenderer.getVertexArray(), 0);
    
    // Vertex attributes should be configured
    // Position: 2 floats, TexCoord: 2 floats, Color: 4 floats = 8 floats total
    EXPECT_EQ(textRenderer.getVertexStride(), sizeof(float) * 8);
}

TEST_F(TextRendererOpenGLTest, SetProjectionMatrix) {
    TextRenderer textRenderer;
    textRenderer.initializeWithShaders();
    
    // Set orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f);
    textRenderer.setProjectionMatrix(projection);
    
    // Should be able to retrieve the projection matrix
    EXPECT_EQ(textRenderer.getProjectionMatrix(), projection);
}

TEST_F(TextRendererOpenGLTest, GenerateProperTextVertices) {
    TextRenderer textRenderer;
    textRenderer.initializeWithShaders();
    
    // Clear any existing vertices
    textRenderer.clearVertices();
    
    // Generate vertices for "Hello"
    textRenderer.generateVerticesForText("Hello", glm::vec2(100, 100), font_.get());
    
    // Should generate 6 vertices per character (2 triangles)
    // "Hello" = 5 characters = 30 vertices
    EXPECT_EQ(textRenderer.getVertexCount(), 30);
    
    // Check first character ('H') vertices are positioned correctly
    auto vertices = textRenderer.getVertices();
    EXPECT_NEAR(vertices[0].position.x, 101.0f, 0.1f);  // 100 + xoffset
    EXPECT_NEAR(vertices[0].position.y, 102.0f, 0.1f);  // 100 + yoffset
}

TEST_F(TextRendererOpenGLTest, HandleMultiLineText) {
    TextRenderer textRenderer;
    textRenderer.initializeWithShaders();
    
    // Render multi-line text
    textRenderer.clearVertices();
    textRenderer.renderMultiLineText("Hello\nWorld", glm::vec2(100, 100), font_.get());
    
    // Should handle newlines properly
    auto vertices = textRenderer.getVertices();
    
    // Find first vertex of second line (character 6, vertex 30)
    if (vertices.size() > 30) {
        // Second line should be below first line by lineHeight
        EXPECT_NEAR(vertices[30].position.y, 120.0f, 1.0f);  // 100 + 20 (lineHeight)
    }
}

TEST_F(TextRendererOpenGLTest, TextAlignmentOptions) {
    TextRenderer textRenderer;
    textRenderer.initializeWithShaders();
    
    // Test right alignment
    textRenderer.setAlignment(TextAlignment::RIGHT);
    textRenderer.clearVertices();
    textRenderer.renderText("Hello", glm::vec2(200, 100), font_.get());
    
    // Text should be positioned to the left of the anchor point
    auto vertices = textRenderer.getVertices();
    if (!vertices.empty()) {
        // Last character should end near x=200
        float lastX = vertices[vertices.size() - 2].position.x;  // Top-right vertex of last char
        EXPECT_NEAR(lastX, 200.0f, 5.0f);
    }
    
    // Test center alignment
    textRenderer.setAlignment(TextAlignment::CENTER);
    textRenderer.clearVertices();
    textRenderer.renderText("Hello", glm::vec2(200, 100), font_.get());
    
    vertices = textRenderer.getVertices();
    if (!vertices.empty()) {
        // Text should be centered around x=200
        float firstX = vertices[0].position.x;
        float lastX = vertices[vertices.size() - 2].position.x;
        float centerX = (firstX + lastX) / 2.0f;
        EXPECT_NEAR(centerX, 200.0f, 10.0f);
    }
}

TEST_F(TextRendererOpenGLTest, RenderToTexture) {
    TextRenderer textRenderer;
    textRenderer.initializeWithShaders();
    
    // Create render target texture
    unsigned int textureId = textRenderer.createRenderTexture(256, 64);
    EXPECT_GT(textureId, 0);
    
    // Render text to texture
    textRenderer.beginRenderToTexture(textureId, 256, 64);
    textRenderer.renderText("Hello World", glm::vec2(10, 32), font_.get());
    textRenderer.endRenderToTexture();
    
    // Texture should contain rendered text
    // In real implementation, we could read back pixels and verify
    EXPECT_GT(textRenderer.getLastRenderTextureId(), 0);
}

} // namespace d2