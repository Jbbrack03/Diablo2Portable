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
    manifestVersion = 1; // Reset to default
    
    std::string line;
    while (std::getline(file, line)) {
        // Look for version line
        if (line.find("\"version\":") != std::string::npos) {
            size_t colonPos = line.find(":");
            size_t commaPos = line.find(",", colonPos);
            std::string versionStr = line.substr(colonPos + 1, commaPos - colonPos - 1);
            // Trim whitespace
            versionStr.erase(0, versionStr.find_first_not_of(" \t"));
            versionStr.erase(versionStr.find_last_not_of(" \t") + 1);
            manifestVersion = std::stoi(versionStr);
        }
        // Look for path lines
        if (line.find("\"path\":") != std::string::npos) {
            size_t start = line.find("\"", line.find("\"path\":") + 7) + 1;
            size_t end = line.find("\"", start);
            std::string assetPath = line.substr(start, end - start);
            
            AssetInfo info;
            info.path = assetPath;
            
            // Read next lines for size, checksum, type, version
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
            
            // Check if there are more fields (type and version)
            // Need to peek at the next line to see if it's continuing the asset
            std::streampos oldPos = file.tellg();
            if (std::getline(file, line)) {
                if (line.find("\"type\":") != std::string::npos) {
                    size_t start = line.find("\"", line.find("\"type\":") + 6) + 1;
                    size_t end = line.find("\"", start);
                    info.type = line.substr(start, end - start);
                    
                    // Check for version on next line
                    if (std::getline(file, line) && line.find("\"version\":") != std::string::npos) {
                        size_t colonPos = line.find(":");
                        std::string afterColon = line.substr(colonPos + 1);
                        // Find first digit
                        size_t digitStart = afterColon.find_first_of("0123456789");
                        if (digitStart != std::string::npos) {
                            size_t digitEnd = afterColon.find_first_not_of("0123456789", digitStart);
                            std::string versionStr = afterColon.substr(digitStart, digitEnd - digitStart);
                            info.version = std::stoi(versionStr);
                        }
                    }
                } else {
                    // This line is not part of the current asset, seek back
                    file.seekg(oldPos);
                }
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