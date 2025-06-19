#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>

// Forward declaration for asset stream
class AssetStream {
public:
    virtual ~AssetStream() = default;
    virtual size_t read(uint8_t* buffer, size_t size) = 0;
    virtual size_t tell() const = 0;
    virtual void seek(size_t position) = 0;
    virtual size_t size() const = 0;
};

/**
 * APK Asset Loader
 * Handles loading assets from Android APK file
 */
class APKAssetLoader {
public:
    APKAssetLoader();
    ~APKAssetLoader();
    
    // Initialize with Android AssetManager
    bool initialize(void* assetManager);
    
    // Check if initialized
    bool isInitialized() const { return initialized_; }
    
    // Load an asset into memory
    bool loadAsset(const std::string& path, std::vector<uint8_t>& data);
    
    // Check if an asset exists
    bool assetExists(const std::string& path) const;
    
    // Get the size of an asset
    size_t getAssetSize(const std::string& path) const;
    
    // List assets in a directory
    std::vector<std::string> listAssets(const std::string& directory) const;
    
    // Open an asset for streaming
    std::unique_ptr<AssetStream> openAssetStream(const std::string& path);
    
private:
    bool initialized_;
    void* assetManager_; // AAssetManager* in real implementation
    
    // Mock data for testing
    struct MockAssetData {
        std::string path;
        std::vector<uint8_t> data;
    };
    std::vector<MockAssetData> mockAssets_;
    
    void setupMockAssets();
};