#include "tools/apk_packager.h"
#include "tools/asset_manifest.h"
#include <filesystem>
#include <fstream>
#include <set>

namespace fs = std::filesystem;

namespace d2 {

APKPackager::APKPackager() {
}

APKPackager::~APKPackager() {
}

void APKPackager::addAsset(const std::string& sourcePath, const std::string& apkPath) {
    if (!fs::exists(sourcePath)) {
        return;
    }
    
    Asset asset;
    asset.sourcePath = sourcePath;
    asset.apkPath = apkPath;
    asset.size = fs::file_size(sourcePath);
    
    assets.push_back(asset);
}

bool APKPackager::packageAssets(const std::string& outputDir, const PackageOptions& options) {
    if (assets.empty()) {
        return true; // Nothing to package
    }
    
    // Create output directory if it doesn't exist
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }
    
    // Create directory structure
    if (!createDirectoryStructure(outputDir)) {
        return false;
    }
    
    // Copy all assets
    for (const auto& asset : assets) {
        if (!copyAsset(asset, outputDir, options)) {
            return false;
        }
    }
    
    return true;
}

void APKPackager::setManifest(std::shared_ptr<AssetManifest> manifest) {
    this->manifest = manifest;
}

size_t APKPackager::getAssetCount() const {
    return assets.size();
}

void APKPackager::clear() {
    assets.clear();
}

size_t APKPackager::getTotalSize() const {
    size_t total = 0;
    for (const auto& asset : assets) {
        total += asset.size;
    }
    return total;
}

bool APKPackager::createDirectoryStructure(const std::string& outputDir) {
    // Create unique directories for all assets
    std::set<fs::path> directories;
    
    for (const auto& asset : assets) {
        fs::path assetPath = fs::path(outputDir) / asset.apkPath;
        directories.insert(assetPath.parent_path());
    }
    
    // Create all directories
    for (const auto& dir : directories) {
        try {
            fs::create_directories(dir);
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    return true;
}

bool APKPackager::copyAsset(const Asset& asset, const std::string& outputDir, const PackageOptions& options) {
    fs::path sourcePath = asset.sourcePath;
    fs::path destPath = fs::path(outputDir) / asset.apkPath;
    
    try {
        // For now, just copy the file. In the future, we can add compression
        fs::copy(sourcePath, destPath, fs::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string APKPackager::getAssetType(const std::string& path) const {
    // TODO: Implement
    return "";
}

} // namespace d2