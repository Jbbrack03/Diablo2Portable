#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace d2 {

/**
 * AssetManifest - Tracks all game assets and their metadata
 * 
 * This class manages a manifest of all game assets, including their
 * file paths, sizes, checksums, and other metadata. Used for:
 * - Asset validation
 * - Download management
 * - Cache invalidation
 * - APK bundling
 */
class AssetManifest {
public:
    struct AssetInfo {
        std::string path;      // Relative path to asset
        size_t size;          // File size in bytes
        std::string checksum; // SHA-256 or similar checksum
        std::string type;     // Asset type (sprite, sound, data, etc.)
        int version;          // Version number for updates
    };
    
    AssetManifest() = default;
    ~AssetManifest() = default;
    
    /**
     * Add an asset to the manifest
     * @param path Relative path to the asset
     * @param size Size of the asset in bytes
     * @param checksum Checksum of the asset for validation
     */
    void addAsset(const std::string& path, size_t size, const std::string& checksum);
    
    /**
     * Save the manifest to a JSON file
     * @param path Path where the manifest will be saved
     * @return true if save succeeded
     */
    bool save(const std::string& path) const;
    
    /**
     * Load a manifest from a JSON file
     * @param path Path to the manifest file
     * @return true if load succeeded
     */
    bool load(const std::string& path);
    
    /**
     * Get the number of assets in the manifest
     * @return Number of assets
     */
    size_t getAssetCount() const { return assets.size(); }
    
    /**
     * Check if an asset exists in the manifest
     * @param path Asset path to check
     * @return true if asset exists
     */
    bool hasAsset(const std::string& path) const {
        return assets.find(path) != assets.end();
    }
    
    /**
     * Get information about a specific asset
     * @param path Asset path
     * @return Asset information or nullptr if not found
     */
    const AssetInfo* getAssetInfo(const std::string& path) const;
    
    /**
     * Get all assets of a specific type
     * @param type Asset type to filter by (e.g., "sprite", "sound")
     * @return List of asset paths matching the type
     */
    std::vector<std::string> getAssetsByType(const std::string& type) const;
    
    /**
     * Calculate total size of all assets
     * @return Total size in bytes
     */
    size_t getTotalSize() const;
    
    /**
     * Clear all assets from the manifest
     */
    void clear() { assets.clear(); }
    
    /**
     * Set the manifest version
     * @param version Version number
     */
    void setVersion(int version) { manifestVersion = version; }
    
    /**
     * Get the manifest version
     * @return Version number
     */
    int getVersion() const { return manifestVersion; }
    
private:
    std::unordered_map<std::string, AssetInfo> assets;
    int manifestVersion = 1;
    
    // Helper methods
    std::string detectAssetType(const std::string& path) const;
};

} // namespace d2