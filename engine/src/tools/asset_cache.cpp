#include "tools/asset_cache.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace d2 {

AssetCache::AssetCache(size_t maxMemoryBytes) 
    : maxMemory(maxMemoryBytes) {
}

std::shared_ptr<std::vector<uint8_t>> AssetCache::loadAsset(const std::string& assetPath) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    
    // Check if asset is already in cache
    auto it = cache.find(assetPath);
    if (it != cache.end()) {
        // Cache hit - move to front of LRU list
        cacheHits++;
        it->second.lastAccess = std::chrono::steady_clock::now();
        
        // Move to front of LRU list
        lruList.remove(assetPath);
        lruList.push_front(assetPath);
        
        return it->second.data;
    }
    
    // Cache miss - load from file
    cacheMisses++;
    
    if (!fs::exists(assetPath)) {
        return nullptr;
    }
    
    std::ifstream file(assetPath, std::ios::binary | std::ios::ate);
    if (!file) {
        return nullptr;
    }
    
    size_t fileSize = file.tellg();
    file.seekg(0);
    
    auto data = std::make_shared<std::vector<uint8_t>>(fileSize);
    file.read(reinterpret_cast<char*>(data->data()), fileSize);
    
    if (!file) {
        return nullptr;
    }
    
    // Add to cache
    CacheEntry entry;
    entry.data = data;
    entry.lastAccess = std::chrono::steady_clock::now();
    
    cache[assetPath] = entry;
    lruList.push_front(assetPath);
    currentMemory += fileSize;
    
    // TODO: Evict old entries if we exceed memory limit
    
    return data;
}

} // namespace d2