#include "tools/apk_packager.h"
#include "tools/asset_manifest.h"
#include <filesystem>
#include <fstream>
#include <set>
#include <zlib.h>
#include <functional>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

namespace d2 {

namespace {
    // Helper function to calculate file checksum using content hash
    std::string calculateFileChecksum(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            return "";
        }
        
        // Read entire file content
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<char> content(size);
        file.read(content.data(), size);
        file.close();
        
        // Calculate hash of content
        std::hash<std::string> hasher;
        size_t hash = hasher(std::string(content.begin(), content.end()));
        
        // Convert hash to hex string
        std::stringstream ss;
        ss << std::hex << hash;
        return ss.str();
    }
}

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

void APKPackager::addAssetDirectory(const std::string& sourceDir, const std::string& apkDir) {
    if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
        return;
    }
    
    // Recursively iterate through all files in the directory
    for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
        if (entry.is_regular_file()) {
            // Calculate relative path from sourceDir
            fs::path relativePath = fs::relative(entry.path(), sourceDir);
            
            // Construct APK path
            fs::path apkPath = fs::path(apkDir) / relativePath;
            
            // Add the asset
            addAsset(entry.path().string(), apkPath.string());
        }
    }
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
    
    // Generate index if requested
    if (options.generateIndex) {
        fs::path indexPath = fs::path(outputDir) / "assets" / "index.json";
        std::ofstream indexFile(indexPath);
        if (!indexFile) {
            return false;
        }
        
        indexFile << "{\n";
        indexFile << "  \"assets\": [\n";
        
        for (size_t i = 0; i < assets.size(); ++i) {
            const auto& asset = assets[i];
            fs::path assetPath = fs::path(asset.apkPath);
            
            indexFile << "    {\n";
            indexFile << "      \"path\": \"" << asset.apkPath << "\",\n";
            indexFile << "      \"name\": \"" << assetPath.filename().string() << "\",\n";
            indexFile << "      \"size\": " << asset.size << ",\n";
            indexFile << "      \"compressed\": " << (options.compressAssets ? "true" : "false") << "\n";
            indexFile << "    }";
            
            if (i < assets.size() - 1) {
                indexFile << ",";
            }
            indexFile << "\n";
        }
        
        indexFile << "  ]\n";
        indexFile << "}\n";
        indexFile.close();
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
    
    // Add .gz extension if compression is enabled
    if (options.compressAssets) {
        destPath += ".gz";
    }
    
    try {
        if (options.compressAssets) {
            // Compress the file using zlib
            std::ifstream input(sourcePath, std::ios::binary);
            if (!input) {
                return false;
            }
            
            // Read entire file into buffer
            input.seekg(0, std::ios::end);
            size_t size = input.tellg();
            input.seekg(0, std::ios::beg);
            
            std::vector<char> buffer(size);
            input.read(buffer.data(), size);
            input.close();
            
            // Compress using zlib
            uLongf compressedSize = compressBound(size);
            std::vector<Bytef> compressed(compressedSize);
            
            int result = compress2(compressed.data(), &compressedSize,
                                   reinterpret_cast<const Bytef*>(buffer.data()),
                                   size, options.compressionLevel);
            
            if (result != Z_OK) {
                return false;
            }
            
            // Write gzip format file
            std::ofstream output(destPath, std::ios::binary);
            if (!output) {
                return false;
            }
            
            // Write gzip header
            const unsigned char gzipHeader[] = {
                0x1f, 0x8b,  // Magic number
                0x08,        // Compression method (deflate)
                0x00,        // Flags
                0x00, 0x00, 0x00, 0x00,  // Timestamp
                0x00,        // Extra flags
                0xff         // OS type
            };
            output.write(reinterpret_cast<const char*>(gzipHeader), sizeof(gzipHeader));
            
            // Write compressed data (skip first 2 bytes of zlib header)
            output.write(reinterpret_cast<const char*>(compressed.data() + 2), compressedSize - 6);
            
            // Calculate CRC32
            uLong crc = crc32(0L, reinterpret_cast<const Bytef*>(buffer.data()), size);
            
            // Write gzip trailer (CRC32 and uncompressed size)
            unsigned char trailer[8];
            trailer[0] = crc & 0xff;
            trailer[1] = (crc >> 8) & 0xff;
            trailer[2] = (crc >> 16) & 0xff;
            trailer[3] = (crc >> 24) & 0xff;
            trailer[4] = size & 0xff;
            trailer[5] = (size >> 8) & 0xff;
            trailer[6] = (size >> 16) & 0xff;
            trailer[7] = (size >> 24) & 0xff;
            output.write(reinterpret_cast<const char*>(trailer), 8);
            
            output.close();
        } else {
            // Just copy the file without compression
            fs::copy(sourcePath, destPath, fs::copy_options::overwrite_existing);
        }
        
        // Update manifest if set
        if (manifest) {
            std::string checksum = calculateFileChecksum(asset.sourcePath);
            size_t fileSize = asset.size;
            manifest->addAsset(asset.apkPath, fileSize, checksum);
            
            // Determine asset type from extension
            std::string type = getAssetType(asset.apkPath);
            
            // Update type in manifest (need to extend manifest API for this)
            auto info = const_cast<AssetManifest::AssetInfo*>(manifest->getAssetInfo(asset.apkPath));
            if (info) {
                info->type = type;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string APKPackager::getAssetType(const std::string& path) const {
    fs::path pathObj(path);
    std::string ext = pathObj.extension().string();
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".png") {
        return "image/png";
    } else if (ext == ".jpg" || ext == ".jpeg") {
        return "image/jpeg";
    } else if (ext == ".ogg") {
        return "audio/ogg";
    } else if (ext == ".mp3") {
        return "audio/mpeg";
    } else if (ext == ".json") {
        return "application/json";
    } else if (ext == ".txt") {
        return "text/plain";
    } else {
        return "application/octet-stream";
    }
}

} // namespace d2