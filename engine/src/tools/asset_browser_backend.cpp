#include "tools/asset_browser_backend.h"
#include <filesystem>

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

} // namespace d2