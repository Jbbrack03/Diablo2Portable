#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace d2 {

/**
 * AssetMetadata - Information about a single game asset
 */
struct AssetMetadata {
    std::string filename;
    std::string category;
    size_t fileSize = 0;
    size_t frameCount = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    std::string fullPath;
};

/**
 * AssetBrowserBackend - Backend service for browsing extracted game assets
 * 
 * This class provides functionality to browse, search, and retrieve information
 * about extracted Diablo II game assets. It supports categorization, metadata
 * extraction, and thumbnail generation for the visual asset browser UI.
 */
class AssetBrowserBackend {
public:
    AssetBrowserBackend() = default;
    ~AssetBrowserBackend() = default;
    
    /**
     * Initialize the asset browser with the root directory of extracted assets
     * @param assetRootPath Path to the root directory containing extracted assets
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(const std::string& assetRootPath);
    
    /**
     * Get metadata for a specific asset
     * @param relativePath Relative path to the asset from the root directory
     * @return AssetMetadata structure with information about the asset
     */
    AssetMetadata getAssetMetadata(const std::string& relativePath) const;
    
private:
    std::string rootPath;
    std::unordered_map<std::string, AssetMetadata> assetCache;
};

} // namespace d2