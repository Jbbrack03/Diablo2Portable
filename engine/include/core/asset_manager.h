#ifndef D2PORTABLE_ASSET_MANAGER_H
#define D2PORTABLE_ASSET_MANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <future>
#include <chrono>
#include "sprites/dc6_parser.h"

// Forward declaration
namespace d2 {
    class MemoryMonitor;
}

namespace d2portable {
namespace core {

/**
 * Asset loading status
 */
enum class AssetStatus {
    NOT_LOADED,
    LOADING,
    LOADED,
    FAILED
};

/**
 * Asset information structure
 */
struct AssetInfo {
    std::string filepath;
    AssetStatus status;
    size_t memory_size;
    std::chrono::time_point<std::chrono::steady_clock> last_accessed;
};

/**
 * Asset Manager for Diablo II game assets
 * 
 * Provides centralized asset loading, caching, and management for
 * game resources including sprites, data files, and other assets.
 */
class AssetManager {
public:
    AssetManager();
    ~AssetManager();
    
    /**
     * Initialize the asset manager with game data path
     * @param data_path Path to the directory containing Diablo II data files
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& data_path);
    
    /**
     * Initialize the asset manager with a single MPQ archive
     * @param mpq_path Path to the MPQ archive file
     * @param fallback_path Optional fallback directory for files not in MPQ
     * @return true if initialization successful, false otherwise
     */
    bool initializeWithMPQ(const std::string& mpq_path, const std::string& fallback_path = "");
    
    /**
     * Initialize the asset manager with a directory containing MPQ archives
     * @param mpq_directory Path to directory containing .mpq files
     * @param fallback_path Optional fallback directory for files not in MPQs
     * @return true if initialization successful, false otherwise
     */
    bool initializeWithMPQs(const std::string& mpq_directory, const std::string& fallback_path = "");
    
    /**
     * Check if the asset manager is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const;
    
    /**
     * Check if a file exists in the asset system
     * @param relative_path Relative path from data directory (e.g., "data/global/ui/panel/invchar6.dc6")
     * @return true if file exists, false otherwise
     */
    bool hasFile(const std::string& relative_path) const;
    
    /**
     * Load a DC6 sprite synchronously
     * @param relative_path Relative path to DC6 file
     * @return Shared pointer to loaded sprite, or nullptr on failure
     */
    std::shared_ptr<sprites::DC6Sprite> loadSprite(const std::string& relative_path);
    
    /**
     * Load a DC6 sprite asynchronously
     * @param relative_path Relative path to DC6 file
     * @return Future that will contain the loaded sprite
     */
    std::future<std::shared_ptr<sprites::DC6Sprite>> loadSpriteAsync(const std::string& relative_path);
    
    /**
     * Load raw file data synchronously
     * @param relative_path Relative path to file
     * @return Vector containing file data, or empty vector on failure
     */
    std::vector<uint8_t> loadFileData(const std::string& relative_path);
    
    /**
     * Get asset information
     * @param relative_path Relative path to asset
     * @return Asset information structure
     */
    AssetInfo getAssetInfo(const std::string& relative_path) const;
    
    /**
     * Get cache statistics
     * @return Number of cached assets
     */
    size_t getCachedAssetCount() const;
    
    /**
     * Get total memory usage of cached assets
     * @return Memory usage in bytes
     */
    size_t getCacheMemoryUsage() const;
    
    /**
     * Clear all cached assets
     */
    void clearCache();
    
    /**
     * Set maximum cache memory usage (0 = unlimited)
     * @param max_bytes Maximum memory in bytes
     */
    void setMaxCacheSize(size_t max_bytes);
    
    /**
     * Get the last error message
     * @return Error message string
     */
    std::string getLastError() const;
    
    /**
     * Set a memory monitor for tracking allocations
     * @param monitor Pointer to MemoryMonitor (can be nullptr to disable)
     */
    void setMemoryMonitor(d2::MemoryMonitor* monitor);
    
    /**
     * Get the current memory monitor
     * @return Pointer to MemoryMonitor or nullptr if not set
     */
    d2::MemoryMonitor* getMemoryMonitor() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace core
} // namespace d2portable

#endif // D2PORTABLE_ASSET_MANAGER_H