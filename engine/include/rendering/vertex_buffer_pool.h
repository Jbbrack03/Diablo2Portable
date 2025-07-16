#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include "rendering/vertex_buffer.h"

namespace d2::rendering {

/**
 * VertexBufferPool - Pool of vertex buffers for performance optimization
 * 
 * Manages a pool of vertex buffers to reduce memory allocations and improve
 * rendering performance by reusing buffers across frames.
 */
class VertexBufferPool {
public:
    /**
     * Constructor
     * @param initialSize Initial number of buffers to allocate
     */
    explicit VertexBufferPool(size_t initialSize);
    
    /**
     * Destructor
     */
    ~VertexBufferPool();
    
    /**
     * Acquire a vertex buffer from the pool
     * @param vertexCount Number of vertices needed
     * @return Shared pointer to a vertex buffer
     */
    std::shared_ptr<VertexBuffer> acquire(size_t vertexCount);
    
    /**
     * Release a vertex buffer back to the pool
     * @param buffer Buffer to release
     */
    void release(std::shared_ptr<VertexBuffer> buffer);
    
    /**
     * Get the current pool size
     * @return Number of buffers in pool
     */
    size_t getPoolSize() const;
    
    /**
     * Get the number of available buffers
     * @return Number of available buffers
     */
    size_t getAvailableCount() const;

private:
    mutable std::mutex mutex_;
    std::vector<std::shared_ptr<VertexBuffer>> buffers_;
    std::vector<std::shared_ptr<VertexBuffer>> availableBuffers_;
    size_t initialSize_;
    
    /**
     * Expand the pool by adding more buffers
     */
    void expandPool();
};

} // namespace d2::rendering