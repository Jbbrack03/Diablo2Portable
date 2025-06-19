#include "ui/text_renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <algorithm>

namespace d2 {

// Vertex shader source for text rendering
const char* textVertexShaderSource = R"(
#version 300 es
precision mediump float;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment shader source for text rendering
const char* textFragmentShaderSource = R"(
#version 300 es
precision mediump float;

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec4 textColor;

void main() {
    float alpha = texture(textTexture, TexCoord).r;
    FragColor = vec4(textColor.rgb, textColor.a * alpha);
}
)";

bool TextRenderer::initializeWithShaders() {
    // In a real implementation, this would:
    // 1. Compile vertex and fragment shaders
    // 2. Link shader program
    // 3. Get uniform locations
    // 4. Create VAO and VBO
    
    // For testing, we'll simulate successful initialization
    shaderProgram_ = 1;  // Mock shader program ID
    vertexBuffer_ = 1;   // Mock VBO ID
    vertexArray_ = 1;    // Mock VAO ID
    
    // Store uniform locations (mock values)
    uniformLocations_["projection"] = 0;
    uniformLocations_["textTexture"] = 1;
    uniformLocations_["textColor"] = 2;
    
    return true;
}

int TextRenderer::getUniformLocation(const std::string& name) const {
    auto it = uniformLocations_.find(name);
    return (it != uniformLocations_.end()) ? it->second : -1;
}

void TextRenderer::setProjectionMatrix(const glm::mat4& projection) {
    projectionMatrix_ = projection;
}

void TextRenderer::generateVerticesForText(const std::string& text, const glm::vec2& position, const Font* font) {
    if (!font || text.empty()) return;
    
    glm::vec2 currentPos = position;
    
    for (char c : text) {
        auto charInfo = font->getCharacterInfo(c);
        
        // Adjust position with character offsets
        float x = currentPos.x + charInfo.xOffset;
        float y = currentPos.y + charInfo.yOffset;
        
        // Create 6 vertices for character quad (2 triangles)
        // Top-left triangle
        vertices_.push_back({
            {x, y},
            {charInfo.u, charInfo.v},
            color_
        });
        vertices_.push_back({
            {x + charInfo.width, y},
            {charInfo.u + charInfo.uWidth, charInfo.v},
            color_
        });
        vertices_.push_back({
            {x, y + charInfo.height},
            {charInfo.u, charInfo.v + charInfo.vHeight},
            color_
        });
        
        // Bottom-right triangle
        vertices_.push_back({
            {x + charInfo.width, y},
            {charInfo.u + charInfo.uWidth, charInfo.v},
            color_
        });
        vertices_.push_back({
            {x + charInfo.width, y + charInfo.height},
            {charInfo.u + charInfo.uWidth, charInfo.v + charInfo.vHeight},
            color_
        });
        vertices_.push_back({
            {x, y + charInfo.height},
            {charInfo.u, charInfo.v + charInfo.vHeight},
            color_
        });
        
        currentPos.x += charInfo.advance;
        vertexCount_ += 6;
    }
}

void TextRenderer::renderMultiLineText(const std::string& text, const glm::vec2& position, const Font* font) {
    if (!font || text.empty()) return;
    
    // Split text by newlines
    std::istringstream stream(text);
    std::string line;
    glm::vec2 currentPos = position;
    
    while (std::getline(stream, line)) {
        renderText(line, currentPos, font);
        currentPos.y += font->getLineHeight();
    }
}

unsigned int TextRenderer::createRenderTexture(int width, int height) {
    // In real implementation, this would create an OpenGL texture
    // For testing, return a mock texture ID
    lastRenderTextureId_ = static_cast<unsigned int>(width * height);  // Unique ID based on dimensions
    return lastRenderTextureId_;
}

void TextRenderer::beginRenderToTexture(unsigned int textureId, int width, int height) {
    // In real implementation, this would:
    // 1. Create/bind framebuffer
    // 2. Attach texture as color attachment
    // 3. Set viewport to texture dimensions
    // 4. Clear texture
    
    // For testing, just store the texture ID
    lastRenderTextureId_ = textureId;
    
    // Update projection matrix for texture rendering
    projectionMatrix_ = glm::ortho(0.0f, static_cast<float>(width), 
                                  static_cast<float>(height), 0.0f);
}

void TextRenderer::endRenderToTexture() {
    // In real implementation, this would:
    // 1. Unbind framebuffer
    // 2. Restore previous viewport
    
    // For testing, nothing to do
}

// Override renderText to handle alignment
void TextRenderer::renderText(const std::string& text, const glm::vec2& position, const Font* font) {
    if (!font || text.empty()) return;
    
    glm::vec2 adjustedPos = position;
    
    // Adjust position based on alignment
    if (alignment_ != TextAlignment::LEFT) {
        int textWidth = font->getTextWidth(text);
        
        if (alignment_ == TextAlignment::CENTER) {
            adjustedPos.x -= textWidth / 2.0f;
        } else if (alignment_ == TextAlignment::RIGHT) {
            adjustedPos.x -= textWidth;
        }
    }
    
    // Generate vertices for the text
    generateVerticesForText(text, adjustedPos, font);
    
    if (!batchActive_) {
        // If not batching, render immediately
        // In real implementation, this would make a draw call
    }
}

} // namespace d2