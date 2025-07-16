#include "android/asset_path_validator.h"
#include <fstream>
#include <algorithm>

namespace d2 {

AssetPathValidator::ValidationResult AssetPathValidator::validateAssetPath(const std::string& assetPath) {
    ValidationResult result;
    
    // Check if path exists
    if (!std::filesystem::exists(assetPath)) {
        result.errorMessage = "Asset path does not exist: " + assetPath;
        return result;
    }
    
    if (!std::filesystem::is_directory(assetPath)) {
        result.errorMessage = "Asset path is not a directory: " + assetPath;
        return result;
    }
    
    // Find all MPQ files
    auto mpqFiles = findMPQFiles(assetPath);
    result.foundFiles = mpqFiles;
    
    // Check for required files
    for (const char* requiredFile : REQUIRED_MPQS) {
        std::filesystem::path fullPath = std::filesystem::path(assetPath) / requiredFile;
        if (!std::filesystem::exists(fullPath) || !isValidMPQFile(fullPath)) {
            result.missingFiles.push_back(requiredFile);
        }
    }
    
    result.isValid = result.missingFiles.empty();
    if (!result.isValid) {
        result.errorMessage = "Missing required MPQ files";
    }
    
    return result;
}

bool AssetPathValidator::hasRequiredMPQFiles(const std::string& assetPath) {
    auto result = validateAssetPath(assetPath);
    return result.isValid;
}

std::vector<std::string> AssetPathValidator::findMPQFiles(const std::string& assetPath) {
    std::vector<std::string> mpqFiles;
    
    if (!std::filesystem::exists(assetPath) || !std::filesystem::is_directory(assetPath)) {
        return mpqFiles;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(assetPath)) {
        if (entry.is_regular_file()) {
            auto path = entry.path();
            if (path.extension() == ".mpq" || path.extension() == ".MPQ") {
                if (isValidMPQFile(path)) {
                    mpqFiles.push_back(path.filename().string());
                }
            }
        }
    }
    
    std::sort(mpqFiles.begin(), mpqFiles.end());
    return mpqFiles;
}

bool AssetPathValidator::isValidMPQFile(const std::filesystem::path& filePath) {
    // Check file size (MPQ files should be at least 32 bytes for header)
    if (std::filesystem::file_size(filePath) < 32) {
        return false;
    }
    
    // Check MPQ signature
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return false;
    }
    
    char signature[4];
    file.read(signature, 4);
    
    // MPQ files start with "MPQ\x1A"
    return file.gcount() == 4 && 
           signature[0] == 'M' && 
           signature[1] == 'P' && 
           signature[2] == 'Q' && 
           signature[3] == '\x1A';
}

} // namespace d2