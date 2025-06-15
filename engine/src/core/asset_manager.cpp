#include "core/asset_manager.h"
#include "utils/mpq_loader.h"
#include "sprites/dc6_parser.h"
#include <filesystem>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <fstream>
#include <algorithm>

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
    Impl() : initialized(false), max_cache_size(0), use_mpq(false) {}
    
    bool initialized;
    std::string data_path;
    std::string last_error;
    size_t max_cache_size;
    bool use_mpq;
    
    // MPQ support
    std::vector<std::unique_ptr<utils::MPQLoader>> mpq_loaders;
    std::string fallback_path;
    
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

bool AssetManager::initializeWithMPQ(const std::string& mpq_path, const std::string& fallback_path) {
    if (!std::filesystem::exists(mpq_path)) {
        pImpl->last_error = "MPQ file does not exist: " + mpq_path;
        return false;
    }
    
    auto loader = std::make_unique<utils::MPQLoader>();
    if (!loader->open(mpq_path)) {
        pImpl->last_error = "Failed to open MPQ: " + loader->getLastError();
        return false;
    }
    
    pImpl->mpq_loaders.clear();
    pImpl->mpq_loaders.push_back(std::move(loader));
    pImpl->use_mpq = true;
    pImpl->fallback_path = fallback_path;
    pImpl->initialized = true;
    pImpl->last_error.clear();
    
    return true;
}

bool AssetManager::initializeWithMPQs(const std::string& mpq_directory, const std::string& fallback_path) {
    if (!std::filesystem::exists(mpq_directory)) {
        pImpl->last_error = "MPQ directory does not exist: " + mpq_directory;
        return false;
    }
    
    if (!std::filesystem::is_directory(mpq_directory)) {
        pImpl->last_error = "Path is not a directory: " + mpq_directory;
        return false;
    }
    
    pImpl->mpq_loaders.clear();
    
    // Find and open all MPQ files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(mpq_directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mpq") {
            auto loader = std::make_unique<utils::MPQLoader>();
            if (loader->open(entry.path().string())) {
                pImpl->mpq_loaders.push_back(std::move(loader));
            }
        }
    }
    
    if (pImpl->mpq_loaders.empty()) {
        pImpl->last_error = "No valid MPQ files found in directory: " + mpq_directory;
        return false;
    }
    
    pImpl->use_mpq = true;
    pImpl->fallback_path = fallback_path;
    pImpl->initialized = true;
    pImpl->last_error.clear();
    
    return true;
}

bool AssetManager::hasFile(const std::string& relative_path) const {
    if (!pImpl->initialized) {
        return false;
    }
    
    // If using MPQ archives, check them first
    if (pImpl->use_mpq) {
        // Convert forward slashes to backslashes for MPQ compatibility
        std::string mpq_path = relative_path;
        std::replace(mpq_path.begin(), mpq_path.end(), '/', '\\');
        
        // Check each loaded MPQ
        for (const auto& loader : pImpl->mpq_loaders) {
            if (loader->hasFile(mpq_path)) {
                return true;
            }
        }
        
        // Check fallback path if set
        if (!pImpl->fallback_path.empty()) {
            std::string fallback_file = (std::filesystem::path(pImpl->fallback_path) / relative_path).string();
            if (std::filesystem::exists(fallback_file)) {
                return true;
            }
        }
        
        return false;
    }
    
    // Original filesystem check
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
    
    sprites::DC6Parser parser;
    std::unique_ptr<sprites::DC6Sprite> sprite;
    
    // Try loading from MPQ first if enabled
    if (pImpl->use_mpq) {
        // Convert forward slashes to backslashes for MPQ compatibility
        std::string mpq_path = relative_path;
        std::replace(mpq_path.begin(), mpq_path.end(), '/', '\\');
        
        // Try each MPQ loader
        for (const auto& loader : pImpl->mpq_loaders) {
            std::vector<uint8_t> data;
            if (loader->extractFile(mpq_path, data)) {
                sprite = parser.parseData(data);
                if (sprite) {
                    break;
                }
            }
        }
        
        // Try fallback path if sprite not found in MPQs
        if (!sprite && !pImpl->fallback_path.empty()) {
            std::string fallback_file = (std::filesystem::path(pImpl->fallback_path) / relative_path).string();
            if (std::filesystem::exists(fallback_file)) {
                sprite = parser.parseFile(fallback_file);
            }
        }
    } else {
        // Original filesystem loading
        std::string full_path = pImpl->resolveFilePath(relative_path);
        if (std::filesystem::exists(full_path)) {
            sprite = parser.parseFile(full_path);
        }
    }
    
    if (!sprite) {
        pImpl->last_error = "Failed to load sprite: " + relative_path;
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
    
    // Try loading from MPQ first if enabled
    if (pImpl->use_mpq) {
        // Convert forward slashes to backslashes for MPQ compatibility
        std::string mpq_path = relative_path;
        std::replace(mpq_path.begin(), mpq_path.end(), '/', '\\');
        
        // Try each MPQ loader
        for (const auto& loader : pImpl->mpq_loaders) {
            std::vector<uint8_t> data;
            if (loader->extractFile(mpq_path, data)) {
                return data;
            }
        }
        
        // Try fallback path if not found in MPQs
        if (!pImpl->fallback_path.empty()) {
            std::string fallback_file = (std::filesystem::path(pImpl->fallback_path) / relative_path).string();
            if (std::filesystem::exists(fallback_file)) {
                std::ifstream file(fallback_file, std::ios::binary | std::ios::ate);
                if (file.is_open()) {
                    auto size = file.tellg();
                    file.seekg(0, std::ios::beg);
                    
                    std::vector<uint8_t> data(size);
                    file.read(reinterpret_cast<char*>(data.data()), size);
                    
                    if (file.good()) {
                        return data;
                    }
                }
            }
        }
        
        pImpl->last_error = "File not found in MPQs or fallback: " + relative_path;
        return {};
    }
    
    // Original filesystem loading
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