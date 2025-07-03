#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace d2 {

/**
 * ThumbnailSize - Predefined thumbnail sizes for asset browser
 */
enum class ThumbnailSize {
    SMALL = 64,
    MEDIUM = 128,
    LARGE = 256
};

/**
 * Thumbnail - Generated thumbnail data for an asset
 */
struct Thumbnail {
    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<uint8_t> data;
    
    bool isValid() const { return width > 0 && height > 0 && !data.empty(); }
};

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
    
    /**
     * Generate a thumbnail for an asset
     * @param relativePath Relative path to the asset from the root directory
     * @param size Desired thumbnail size
     * @return Thumbnail structure with image data
     */
    Thumbnail generateThumbnail(const std::string& relativePath, ThumbnailSize size) const;
    
    /**
     * Get list of available asset categories
     * @return Vector of category names found in the asset directory
     */
    std::vector<std::string> getAssetCategories() const;
    
    /**
     * Search for assets by name
     * @param query Search query to match against asset filenames
     * @return Vector of AssetMetadata for matching assets
     */
    std::vector<AssetMetadata> searchAssets(const std::string& query) const;
    
private:
    std::string rootPath;
    std::unordered_map<std::string, AssetMetadata> assetCache;
};

} // namespace d2