#include "rendering/vertex_buffer.h"

namespace d2::rendering {

VertexBuffer::VertexBuffer() = default;

VertexBuffer::~VertexBuffer() {
    release();
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : bufferId_(other.bufferId_)
    , vertexCount_(other.vertexCount_) {
    other.bufferId_ = 0;
    other.vertexCount_ = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
    if (this != &other) {
        release();
        bufferId_ = other.bufferId_;
        vertexCount_ = other.vertexCount_;
        other.bufferId_ = 0;
        other.vertexCount_ = 0;
    }
    return *this;
}

bool VertexBuffer::create(const std::vector<SpriteVertex>& vertices) {
    if (vertices.empty()) {
        return false;
    }
    
    // Release any existing buffer
    release();
    
    vertexCount_ = vertices.size();
    
    // STUB IMPLEMENTATION: Simulate buffer creation
    // In a real implementation, this would:
    // 1. Call glGenBuffers(1, &bufferId_)
    // 2. Call glBindBuffer(GL_ARRAY_BUFFER, bufferId_)
    // 3. Call glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)
    
    static uint32_t nextBufferId = 1;
    bufferId_ = nextBufferId++;
    
    return true;
}

bool VertexBuffer::update(const std::vector<SpriteVertex>& vertices) {
    if (!isValid() || vertices.empty()) {
        return false;
    }
    
    // STUB IMPLEMENTATION
    // In a real implementation, this would:
    // 1. Call glBindBuffer(GL_ARRAY_BUFFER, bufferId_)
    // 2. Call glBufferSubData(GL_ARRAY_BUFFER, 0, size, data)
    
    vertexCount_ = vertices.size();
    return true;
}

void VertexBuffer::bind() const {
    if (!isValid()) {
        return;
    }
    
    // STUB IMPLEMENTATION
    // In a real implementation, this would:
    // Call glBindBuffer(GL_ARRAY_BUFFER, bufferId_)
}

void VertexBuffer::unbind() {
    // STUB IMPLEMENTATION
    // In a real implementation, this would:
    // Call glBindBuffer(GL_ARRAY_BUFFER, 0)
}

void VertexBuffer::release() {
    if (bufferId_ != 0) {
        // STUB IMPLEMENTATION
        // In a real implementation, this would:
        // Call glDeleteBuffers(1, &bufferId_)
        
        bufferId_ = 0;
        vertexCount_ = 0;
    }
}

} // namespace d2::rendering