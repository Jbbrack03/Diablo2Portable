#include "rendering/vertex_buffer_pool.h"
#include <algorithm>

namespace d2::rendering {

VertexBufferPool::VertexBufferPool(size_t initialSize) : initialSize_(initialSize) {
    buffers_.reserve(initialSize * 2); // Reserve space for expansion
    
    // Create initial buffers
    for (size_t i = 0; i < initialSize; ++i) {
        auto buffer = std::make_shared<VertexBuffer>();
        buffers_.push_back(buffer);
        availableBuffers_.push_back(buffer);
    }
}

VertexBufferPool::~VertexBufferPool() = default;

std::shared_ptr<VertexBuffer> VertexBufferPool::acquire(size_t vertexCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // If no buffers available, expand the pool
    if (availableBuffers_.empty()) {
        expandPool();
    }
    
    // Get a buffer from the available pool (use back for LIFO behavior)
    auto buffer = availableBuffers_.back();
    availableBuffers_.pop_back();
    
    // Resize buffer if needed (in a real implementation, we'd check capacity)
    // For now, just assume it can handle the vertex count
    (void)vertexCount; // Suppress unused parameter warning
    
    return buffer;
}

void VertexBufferPool::release(std::shared_ptr<VertexBuffer> buffer) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Return buffer to available pool (use push_back for LIFO behavior)
    availableBuffers_.push_back(buffer);
}

size_t VertexBufferPool::getPoolSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffers_.size();
}

size_t VertexBufferPool::getAvailableCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return availableBuffers_.size();
}

void VertexBufferPool::expandPool() {
    // Add more buffers to the pool
    size_t expandBy = std::max(initialSize_ / 2, size_t(1));
    
    for (size_t i = 0; i < expandBy; ++i) {
        auto buffer = std::make_shared<VertexBuffer>();
        buffers_.push_back(buffer);
        availableBuffers_.push_back(buffer);
    }
}

} // namespace d2::rendering