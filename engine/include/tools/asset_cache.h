#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <list>
#include <chrono>
#include <vector>
#include <mutex>

namespace d2 {

/**
 * AssetCache - LRU (Least Recently Used) cache for game assets
 * 
 * Features:
 * - Memory-limited cache with automatic eviction
 * - Thread-safe access
 * - Performance statistics (hits/misses)
 * - Preloading support for commonly used assets
 * - Memory-mapped file support for large assets
 */
class AssetCache {
public:
    /**
     * Create cache with specified memory limit
     * @param maxMemoryBytes Maximum memory usage in bytes
     */
    explicit AssetCache(size_t maxMemoryBytes);
    ~AssetCache() = default;
    
    /**
     * Get maximum memory limit
     */
    size_t getMaxMemory() const { return maxMemory; }
    
    /**
     * Get current memory usage
     */
    size_t getCurrentMemory() const { return currentMemory; }
    
    /**
     * Get number of cache hits
     */
    size_t getCacheHits() const { return cacheHits; }
    
    /**
     * Get number of cache misses
     */
    size_t getCacheMisses() const { return cacheMisses; }
    
private:
    size_t maxMemory;
    size_t currentMemory = 0;
    size_t cacheHits = 0;
    size_t cacheMisses = 0;
};

} // namespace d2