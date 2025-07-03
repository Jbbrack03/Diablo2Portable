#include "tools/differential_extractor.h"
#include "tools/asset_manifest.h"
#include "tools/asset_extractor.h"
#include <filesystem>
#include <fstream>
#include <chrono>

namespace fs = std::filesystem;

namespace d2 {

bool DifferentialExtractor::fullExtraction(const std::string& d2Path, const std::string& outputPath) {
    // For testing, just create the output path
    fs::create_directories(outputPath);
    
    // Copy MPQ files to output for testing
    if (fs::exists(d2Path)) {
        for (const auto& entry : fs::directory_iterator(d2Path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".mpq") {
                fs::copy_file(entry.path(), outputPath / entry.path().filename(), 
                             fs::copy_options::overwrite_existing);
            }
        }
    }
    
    return true;
}

std::unique_ptr<AssetManifest> DifferentialExtractor::generateManifest(const std::string& extractedPath) {
    auto manifest = std::make_unique<AssetManifest>();
    
    // Scan extracted files and add to manifest
    if (fs::exists(extractedPath)) {
        for (const auto& entry : fs::recursive_directory_iterator(extractedPath)) {
            if (entry.is_regular_file()) {
                fs::path relativePath = fs::relative(entry.path(), extractedPath);
                
                // Calculate file size
                auto fileSize = entry.file_size();
                
                // Calculate simple checksum (in real implementation, would use proper hash)
                auto modTime = fs::last_write_time(entry.path()).time_since_epoch().count();
                std::string checksum = std::to_string(fileSize) + "_" + 
                                      std::to_string(static_cast<long long>(modTime));
                
                manifest->addAsset(relativePath.string(), fileSize, checksum);
            }
        }
    }
    
    return manifest;
}

FileChanges DifferentialExtractor::detectChanges(const std::string& d2Path, 
                                               const AssetManifest& baseManifest) {
    FileChanges changes;
    
    // Check MPQ files in the D2 path
    if (fs::exists(d2Path)) {
        for (const auto& entry : fs::directory_iterator(d2Path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".mpq") {
                std::string filename = entry.path().filename().string();
                
                // Calculate current file checksum
                auto fileSize = entry.file_size();
                auto modTime = fs::last_write_time(entry.path()).time_since_epoch().count();
                std::string currentChecksum = std::to_string(fileSize) + "_" + 
                                            std::to_string(static_cast<long long>(modTime));
                
                // Check if file exists in manifest
                auto assetInfo = baseManifest.getAssetInfo(filename);
                if (assetInfo->path.empty()) {
                    // File is new
                    changes.addedFiles.push_back(filename);
                } else if (assetInfo->checksum != currentChecksum) {
                    // File has been modified
                    changes.modifiedFiles.push_back(filename);
                }
                // Note: We don't check for unchanged files in this simple implementation
            }
        }
    }
    
    // Check for deleted files would require iterating the manifest
    // and checking if files still exist - skipping for now
    
    return changes;
}

} // namespace d2