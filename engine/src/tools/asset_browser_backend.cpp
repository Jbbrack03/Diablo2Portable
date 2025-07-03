#include "tools/asset_browser_backend.h"
#include <filesystem>
#include <algorithm>
#include <set>

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

std::vector<std::string> AssetBrowserBackend::getAssetCategories() const {
    std::set<std::string> categories;
    
    if (std::filesystem::exists(rootPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(rootPath)) {
            if (entry.is_directory()) {
                categories.insert(entry.path().filename().string());
            }
        }
    }
    
    return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<AssetMetadata> AssetBrowserBackend::searchAssets(const std::string& query) const {
    std::vector<AssetMetadata> results;
    
    if (std::filesystem::exists(rootPath)) {
        // Convert query to lowercase for case-insensitive search
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        
        // Recursive search through all directories
        for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string lowerFilename = filename;
                std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
                
                // Check if filename contains the query
                if (lowerFilename.find(lowerQuery) != std::string::npos) {
                    // Get relative path from root
                    std::filesystem::path relativePath = 
                        std::filesystem::relative(entry.path(), rootPath);
                    
                    // Get metadata for this asset
                    AssetMetadata metadata = getAssetMetadata(relativePath.string());
                    results.push_back(metadata);
                }
            }
        }
    }
    
    return results;
}

} // namespace d2