#include "tools/apk_packager.h"
#include "tools/asset_manifest.h"
#include <filesystem>
#include <fstream>

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
    // TODO: Implement
    return false;
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
    // TODO: Implement
    return false;
}

bool APKPackager::copyAsset(const Asset& asset, const std::string& outputDir, const PackageOptions& options) {
    // TODO: Implement
    return false;
}

std::string APKPackager::getAssetType(const std::string& path) const {
    // TODO: Implement
    return "";
}

} // namespace d2