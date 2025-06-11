#include "core/asset_manager.h"
#include "utils/mpq_loader.h"
#include "sprites/dc6_parser.h"
#include <filesystem>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <fstream>

namespace d2portable {
namespace core {

// Asset cache entry
struct CacheEntry {
    std::shared_ptr<sprites::DC6Sprite> sprite;
    std::vector<uint8_t> raw_data;
    size_t memory_size;
    std::chrono::time_point<std::chrono::steady_clock> last_accessed;
    AssetStatus status;
};

// Private implementation class
class AssetManager::Impl {
public:
    Impl() : initialized(false), max_cache_size(0) {}
    
    bool initialized;
    std::string data_path;
    std::string last_error;
    size_t max_cache_size;
    
    // Cache management
    std::unordered_map<std::string, CacheEntry> cache;
    mutable std::mutex cache_mutex;
    
    // Utility methods
    std::string resolveFilePath(const std::string& relative_path) const {
        return (std::filesystem::path(data_path) / relative_path).string();
    }
    
    void updateLastAccessed(const std::string& path) {
        auto it = cache.find(path);
        if (it != cache.end()) {
            it->second.last_accessed = std::chrono::steady_clock::now();
        }
    }
    
    void enforceCacheLimit() {
        if (max_cache_size == 0) return; // No limit
        
        size_t current_size = 0;
        for (const auto& pair : cache) {
            current_size += pair.second.memory_size;
        }
        
        // If over limit, remove oldest entries
        while (current_size > max_cache_size && !cache.empty()) {
            auto oldest = cache.begin();
            auto oldest_time = oldest->second.last_accessed;
            
            for (auto it = cache.begin(); it != cache.end(); ++it) {
                if (it->second.last_accessed < oldest_time) {
                    oldest = it;
                    oldest_time = it->second.last_accessed;
                }
            }
            
            current_size -= oldest->second.memory_size;
            cache.erase(oldest);
        }
    }
};

AssetManager::AssetManager() : pImpl(std::make_unique<Impl>()) {}

AssetManager::~AssetManager() = default;

bool AssetManager::initialize(const std::string& data_path) {
    if (!std::filesystem::exists(data_path)) {
        pImpl->last_error = "Data path does not exist: " + data_path;
        return false;
    }
    
    if (!std::filesystem::is_directory(data_path)) {
        pImpl->last_error = "Data path is not a directory: " + data_path;
        return false;
    }
    
    pImpl->data_path = data_path;
    pImpl->initialized = true;
    pImpl->last_error.clear();
    
    return true;
}

bool AssetManager::isInitialized() const {
    return pImpl->initialized;
}

bool AssetManager::hasFile(const std::string& relative_path) const {
    if (!pImpl->initialized) {
        return false;
    }
    
    std::string full_path = pImpl->resolveFilePath(relative_path);
    return std::filesystem::exists(full_path);
}

std::shared_ptr<sprites::DC6Sprite> AssetManager::loadSprite(const std::string& relative_path) {
    if (!pImpl->initialized) {
        pImpl->last_error = "Asset manager not initialized";
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->cache_mutex);
    
    // Check cache first
    auto cache_it = pImpl->cache.find(relative_path);
    if (cache_it != pImpl->cache.end() && cache_it->second.sprite) {
        pImpl->updateLastAccessed(relative_path);
        return cache_it->second.sprite;
    }
    
    // Load sprite from file
    std::string full_path = pImpl->resolveFilePath(relative_path);
    if (!std::filesystem::exists(full_path)) {
        pImpl->last_error = "File not found: " + relative_path;
        return nullptr;
    }
    
    sprites::DC6Parser parser;
    auto sprite = parser.parseFile(full_path);
    
    if (!sprite) {
        pImpl->last_error = "Failed to parse DC6 file: " + relative_path;
        return nullptr;
    }
    
    // Convert unique_ptr to shared_ptr properly
    std::shared_ptr<sprites::DC6Sprite> shared_sprite = std::move(sprite);
    
    // Add to cache
    CacheEntry entry;
    entry.sprite = shared_sprite;
    entry.status = AssetStatus::LOADED;
    entry.last_accessed = std::chrono::steady_clock::now();
    
    // Estimate memory size (rough approximation)
    entry.memory_size = 1024; // Base overhead
    try {
        uint32_t dirs = shared_sprite->getDirectionCount();
        uint32_t frames = shared_sprite->getFramesPerDirection();
        if (dirs > 0 && frames > 0) {
            auto dc6_frame = shared_sprite->getFrame(0, 0);
            size_t frame_size = dc6_frame.width * dc6_frame.height * 4; // RGBA
            entry.memory_size += frame_size * dirs * frames;
        }
    } catch (...) {
        // If there's any issue calculating size, just use base overhead
        entry.memory_size = 1024;
    }
    
    pImpl->cache[relative_path] = entry;
    pImpl->enforceCacheLimit();
    
    return shared_sprite;
}

std::future<std::shared_ptr<sprites::DC6Sprite>> AssetManager::loadSpriteAsync(const std::string& relative_path) {
    return std::async(std::launch::async, [this, relative_path]() {
        return loadSprite(relative_path);
    });
}

std::vector<uint8_t> AssetManager::loadFileData(const std::string& relative_path) {
    if (!pImpl->initialized) {
        pImpl->last_error = "Asset manager not initialized";
        return {};
    }
    
    std::string full_path = pImpl->resolveFilePath(relative_path);
    if (!std::filesystem::exists(full_path)) {
        pImpl->last_error = "File not found: " + relative_path;
        return {};
    }
    
    std::ifstream file(full_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        pImpl->last_error = "Failed to open file: " + relative_path;
        return {};
    }
    
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    if (!file.good()) {
        pImpl->last_error = "Failed to read file: " + relative_path;
        return {};
    }
    
    return data;
}

AssetInfo AssetManager::getAssetInfo(const std::string& relative_path) const {
    AssetInfo info;
    info.filepath = relative_path;
    info.status = AssetStatus::NOT_LOADED;
    info.memory_size = 0;
    info.last_accessed = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->cache_mutex);
    
    auto cache_it = pImpl->cache.find(relative_path);
    if (cache_it != pImpl->cache.end()) {
        info.status = cache_it->second.status;
        info.memory_size = cache_it->second.memory_size;
        info.last_accessed = cache_it->second.last_accessed;
    }
    
    return info;
}

size_t AssetManager::getCachedAssetCount() const {
    std::lock_guard<std::mutex> lock(pImpl->cache_mutex);
    return pImpl->cache.size();
}

size_t AssetManager::getCacheMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->cache_mutex);
    
    size_t total = 0;
    for (const auto& pair : pImpl->cache) {
        total += pair.second.memory_size;
    }
    return total;
}

void AssetManager::clearCache() {
    std::lock_guard<std::mutex> lock(pImpl->cache_mutex);
    pImpl->cache.clear();
}

void AssetManager::setMaxCacheSize(size_t max_bytes) {
    std::lock_guard<std::mutex> lock(pImpl->cache_mutex);
    pImpl->max_cache_size = max_bytes;
    pImpl->enforceCacheLimit();
}

std::string AssetManager::getLastError() const {
    return pImpl->last_error;
}

} // namespace core
} // namespace d2portable