#pragma once

#include <string>
#include <vector>
#include <memory>

namespace d2 {

class AssetManifest;

/**
 * APKPackager - Packages optimized assets into Android APK structure
 * 
 * This class handles the packaging of game assets into the Android APK
 * format, organizing them for efficient loading on mobile devices.
 * Works with:
 * - AssetOptimizer output (compressed sprites)
 * - TextureAtlasGenerator output (atlas pages)
 * - AssetManifest for tracking assets
 */
class APKPackager {
public:
    struct PackageOptions {
        bool compressAssets;      // Whether to compress assets in APK
        bool generateIndex;       // Generate asset index for fast lookup
        std::string targetABI;    // Target ABI (arm64-v8a, armeabi-v7a, all)
        int compressionLevel;     // Compression level (1-9)
        
        PackageOptions() 
            : compressAssets(true)
            , generateIndex(true)
            , targetABI("all")
            , compressionLevel(6) {}
    };
    
    APKPackager();
    ~APKPackager();
    
    /**
     * Add an asset file to be packaged
     * @param sourcePath Path to the asset file
     * @param apkPath Relative path within APK (e.g., "assets/sprites/player.png")
     */
    void addAsset(const std::string& sourcePath, const std::string& apkPath);
    
    /**
     * Package all added assets into APK structure
     * @param outputDir Directory where APK assets will be created
     * @param options Packaging options
     * @return true if packaging succeeded
     */
    bool packageAssets(const std::string& outputDir, const PackageOptions& options = {});
    
    /**
     * Set the asset manifest to use for tracking
     * @param manifest Asset manifest instance
     */
    void setManifest(std::shared_ptr<AssetManifest> manifest);
    
    /**
     * Get the number of assets queued for packaging
     * @return Number of assets
     */
    size_t getAssetCount() const;
    
    /**
     * Clear all queued assets
     */
    void clear();
    
    /**
     * Get total size of all assets (uncompressed)
     * @return Total size in bytes
     */
    size_t getTotalSize() const;
    
private:
    struct Asset {
        std::string sourcePath;
        std::string apkPath;
        size_t size;
    };
    
    std::vector<Asset> assets;
    std::shared_ptr<AssetManifest> manifest;
    
    // Helper methods
    bool createDirectoryStructure(const std::string& outputDir);
    bool copyAsset(const Asset& asset, const std::string& outputDir, const PackageOptions& options);
    std::string getAssetType(const std::string& path) const;
};

} // namespace d2