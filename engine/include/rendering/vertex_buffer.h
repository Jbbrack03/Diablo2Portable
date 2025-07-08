#pragma once

#include <vector>
#include <cstdint>
#include <glm/vec2.hpp>

namespace d2::rendering {

// Vertex structure for sprite rendering
struct SpriteVertex {
    glm::vec2 position;
    glm::vec2 texCoord;
};

// Class to manage OpenGL vertex buffers
class VertexBuffer {
public:
    VertexBuffer();
    ~VertexBuffer();
    
    // Prevent copying
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    
    // Allow moving
    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;
    
    // Create buffer and upload data to GPU
    bool create(const std::vector<SpriteVertex>& vertices);
    
    // Update buffer data
    bool update(const std::vector<SpriteVertex>& vertices);
    
    // Bind the buffer for rendering
    void bind() const;
    
    // Unbind any vertex buffer
    static void unbind();
    
    // Get buffer info
    size_t getVertexCount() const { return vertexCount_; }
    bool isValid() const { return bufferId_ != 0; }
    uint32_t getBufferId() const { return bufferId_; }
    
    // Release GPU resources
    void release();
    
private:
    uint32_t bufferId_ = 0;
    size_t vertexCount_ = 0;
};

} // namespace d2::rendering