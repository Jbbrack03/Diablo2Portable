#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "ui/font.h"

namespace d2 {

struct TextVertex {
    glm::vec2 position;
    glm::vec2 texCoord;
    glm::vec4 color;
};

class TextRenderer {
public:
    TextRenderer() : shaderProgram_(0), vertexBuffer_(0), vertexArray_(0), 
                     vertexCount_(0), batchActive_(false) {}
    virtual ~TextRenderer() = default;
    
    const glm::vec4& getColor() const { return color_; }
    void setColor(const glm::vec4& color) { color_ = color; }
    
    // OpenGL resource management
    bool initialize() {
        // Mock OpenGL initialization
        // In real implementation, this would create shaders, VBOs, VAOs
        shaderProgram_ = 1;  // Mock shader program ID
        vertexBuffer_ = 1;   // Mock VBO ID
        vertexArray_ = 1;    // Mock VAO ID
        return true;
    }
    
    unsigned int getShaderProgram() const { return shaderProgram_; }
    unsigned int getVertexBuffer() const { return vertexBuffer_; }
    unsigned int getVertexArray() const { return vertexArray_; }
    int getVertexCount() const { return vertexCount_; }
    
    // Batching support for efficient rendering
    void beginBatch() {
        batchActive_ = true;
        vertices_.clear();
        vertexCount_ = 0;
    }
    
    int endBatch() {
        if (!batchActive_) return 0;
        
        // In real implementation, this would:
        // 1. Upload vertices to VBO
        // 2. Set up shader uniforms
        // 3. Make draw call
        // 4. Return number of draw calls made
        
        batchActive_ = false;
        return vertices_.empty() ? 0 : 1;  // Return 1 draw call if we have vertices
    }
    
    virtual void renderText(const std::string& text, const glm::vec2& position, const Font* font) {
        if (!font || text.empty()) return;
        
        // Generate vertices for each character in the text
        generateTextVertices(text, position, font);
        
        if (!batchActive_) {
            // If not batching, render immediately
            // In real implementation, this would make a draw call
        }
    }
    
private:
    void generateTextVertices(const std::string& text, const glm::vec2& position, const Font* font) {
        glm::vec2 currentPos = position;
        
        for (char c : text) {
            auto charInfo = font->getCharacterInfo(c);
            
            // Create 6 vertices for 2 triangles (quad) per character
            // Top-left triangle
            vertices_.push_back({
                {currentPos.x, currentPos.y},
                {charInfo.u, charInfo.v},
                color_
            });
            vertices_.push_back({
                {currentPos.x + charInfo.width, currentPos.y},
                {charInfo.u + charInfo.uWidth, charInfo.v},
                color_
            });
            vertices_.push_back({
                {currentPos.x, currentPos.y + charInfo.height},
                {charInfo.u, charInfo.v + charInfo.vHeight},
                color_
            });
            
            // Bottom-right triangle
            vertices_.push_back({
                {currentPos.x + charInfo.width, currentPos.y},
                {charInfo.u + charInfo.uWidth, charInfo.v},
                color_
            });
            vertices_.push_back({
                {currentPos.x + charInfo.width, currentPos.y + charInfo.height},
                {charInfo.u + charInfo.uWidth, charInfo.v + charInfo.vHeight},
                color_
            });
            vertices_.push_back({
                {currentPos.x, currentPos.y + charInfo.height},
                {charInfo.u, charInfo.v + charInfo.vHeight},
                color_
            });
            
            currentPos.x += charInfo.advance;
            vertexCount_ += 6;
        }
    }
    
    glm::vec4 color_{1.0f, 1.0f, 1.0f, 1.0f};  // Default white
    
    // OpenGL resources (mock IDs for testing)
    unsigned int shaderProgram_;
    unsigned int vertexBuffer_;
    unsigned int vertexArray_;
    
    // Vertex data for batching
    std::vector<TextVertex> vertices_;
    int vertexCount_;
    bool batchActive_;
};

} // namespace d2