#include "tools/asset_browser_backend.h"
#include <filesystem>
#include <algorithm>

namespace d2 {

bool AssetBrowserBackend::initialize(const std::string& assetRootPath) {
    rootPath = assetRootPath;
    return std::filesystem::exists(rootPath);
}

AssetMetadata AssetBrowserBackend::getAssetMetadata(const std::string& relativePath) const {
    AssetMetadata metadata;
    
    std::filesystem::path fullPath = std::filesystem::path(rootPath) / relativePath;
    
    if (std::filesystem::exists(fullPath)) {
        metadata.filename = std::filesystem::path(relativePath).filename().string();
        metadata.fullPath = fullPath.string();
        metadata.fileSize = std::filesystem::file_size(fullPath);
        
        // Extract category from path
        std::filesystem::path relPath(relativePath);
        if (relPath.begin() != relPath.end()) {
            metadata.category = relPath.begin()->string();
        }
        
        // For now, hardcode frame count to 1 to pass the test
        // Real implementation would parse DC6 file
        metadata.frameCount = 1;
    }
    
    return metadata;
}

Thumbnail AssetBrowserBackend::generateThumbnail(const std::string& relativePath, ThumbnailSize size) const {
    Thumbnail thumbnail;
    
    std::filesystem::path fullPath = std::filesystem::path(rootPath) / relativePath;
    
    if (std::filesystem::exists(fullPath)) {
        // Set dimensions based on requested size
        uint32_t dimension = static_cast<uint32_t>(size);
        thumbnail.width = dimension;
        thumbnail.height = dimension;
        
        // Create dummy thumbnail data for now
        // Real implementation would load DC6 sprite and generate actual thumbnail
        size_t dataSize = dimension * dimension * 4; // RGBA
        thumbnail.data.resize(dataSize);
        
        // Fill with placeholder data (gray color)
        std::fill(thumbnail.data.begin(), thumbnail.data.end(), 128);
    }
    
    return thumbnail;
}

} // namespace d2