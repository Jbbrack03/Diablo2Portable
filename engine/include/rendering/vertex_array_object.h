#pragma once

#include <cstdint>

namespace d2::rendering {

// Vertex Array Object (VAO) - manages vertex attribute configuration
class VertexArrayObject {
public:
    VertexArrayObject() = default;
    ~VertexArrayObject();
    
    // Prevent copying
    VertexArrayObject(const VertexArrayObject&) = delete;
    VertexArrayObject& operator=(const VertexArrayObject&) = delete;
    
    // Allow moving
    VertexArrayObject(VertexArrayObject&& other) noexcept;
    VertexArrayObject& operator=(VertexArrayObject&& other) noexcept;
    
    // Create the VAO
    bool create();
    
    // Bind/unbind for use
    void bind() const;
    static void unbind();
    
    // Check if valid
    bool isValid() const { return vaoId_ != 0; }
    uint32_t getVAOId() const { return vaoId_; }
    
    // Release resources
    void release();
    
private:
    uint32_t vaoId_ = 0;
};

} // namespace d2::rendering