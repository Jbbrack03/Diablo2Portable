#include "tools/asset_manifest.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace d2 {

void AssetManifest::addAsset(const std::string& path, size_t size, const std::string& checksum) {
    AssetInfo info;
    info.path = path;
    info.size = size;
    info.checksum = checksum;
    info.type = detectAssetType(path);
    info.version = 1;
    
    assets[path] = info;
}

bool AssetManifest::save(const std::string& path) const {
    // For now, use a simple text format
    // In production, we'd use a proper JSON library
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    file << "  \"version\": " << manifestVersion << ",\n";
    file << "  \"assets\": [\n";
    
    bool first = true;
    for (const auto& [assetPath, info] : assets) {
        if (!first) file << ",\n";
        file << "    {\n";
        file << "      \"path\": \"" << info.path << "\",\n";
        file << "      \"size\": " << info.size << ",\n";
        file << "      \"checksum\": \"" << info.checksum << "\",\n";
        file << "      \"type\": \"" << info.type << "\",\n";
        file << "      \"version\": " << info.version << "\n";
        file << "    }";
        first = false;
    }
    
    file << "\n  ]\n";
    file << "}\n";
    
    return true;
}

bool AssetManifest::load(const std::string& path) {
    if (!fs::exists(path)) {
        return false;
    }
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing for testing
    // In production, use a proper JSON library
    assets.clear();
    
    std::string line;
    while (std::getline(file, line)) {
        // Look for path lines
        if (line.find("\"path\":") != std::string::npos) {
            size_t start = line.find("\"", line.find("\"path\":") + 7) + 1;
            size_t end = line.find("\"", start);
            std::string assetPath = line.substr(start, end - start);
            
            AssetInfo info;
            info.path = assetPath;
            
            // Read next lines for size and checksum
            std::getline(file, line); // size line
            if (line.find("\"size\":") != std::string::npos) {
                size_t colonPos = line.find(":");
                size_t commaPos = line.find(",", colonPos);
                std::string sizeStr = line.substr(colonPos + 1, commaPos - colonPos - 1);
                info.size = std::stoull(sizeStr);
            }
            
            std::getline(file, line); // checksum line
            if (line.find("\"checksum\":") != std::string::npos) {
                size_t start = line.find("\"", line.find("\"checksum\":") + 10) + 1;
                size_t end = line.find("\"", start);
                info.checksum = line.substr(start, end - start);
            }
            
            assets[assetPath] = info;
        }
    }
    
    return true;
}

const AssetManifest::AssetInfo* AssetManifest::getAssetInfo(const std::string& path) const {
    auto it = assets.find(path);
    return (it != assets.end()) ? &it->second : nullptr;
}

std::vector<std::string> AssetManifest::getAssetsByType(const std::string& type) const {
    std::vector<std::string> result;
    for (const auto& [path, info] : assets) {
        if (info.type == type) {
            result.push_back(path);
        }
    }
    return result;
}

size_t AssetManifest::getTotalSize() const {
    size_t total = 0;
    for (const auto& [path, info] : assets) {
        total += info.size;
    }
    return total;
}

std::string AssetManifest::detectAssetType(const std::string& path) const {
    fs::path p(path);
    std::string ext = p.extension().string();
    
    if (ext == ".png" || ext == ".jpg" || ext == ".webp" || ext == ".pvr") {
        return "sprite";
    } else if (ext == ".ogg" || ext == ".mp3" || ext == ".wav") {
        return "sound";
    } else if (ext == ".json" || ext == ".txt" || ext == ".dat") {
        return "data";
    } else {
        return "unknown";
    }
}

} // namespace d2