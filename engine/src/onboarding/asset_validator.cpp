#include "onboarding/asset_validator.h"
#include "onboarding/file_source_detector.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace d2 {

AssetValidation AssetValidator::validateAssets(const std::string& assetPath) {
    AssetValidation validation;
    
    // Define required core files
    std::vector<std::string> coreFiles = {
        "d2data.mpq",
        "d2sfx.mpq", 
        "d2speech.mpq"
    };
    
    // Define expansion files
    std::vector<std::string> expansionFiles = {
        "d2exp.mpq"
    };
    
    // Check for core files
    fs::path basePath(assetPath);
    for (const auto& fileName : coreFiles) {
        fs::path filePath = basePath / fileName;
        if (!fs::exists(filePath)) {
            validation.missingFiles.push_back(fileName);
        }
    }
    
    // Check for expansion files
    bool hasExpansion = true;
    for (const auto& fileName : expansionFiles) {
        fs::path filePath = basePath / fileName;
        if (!fs::exists(filePath)) {
            validation.missingFiles.push_back(fileName);
            hasExpansion = false;
        }
    }
    
    validation.hasExpansion = hasExpansion;
    validation.isComplete = validation.missingFiles.empty();
    validation.version = hasExpansion ? D2Version::LORD_OF_DESTRUCTION : D2Version::CLASSIC;
    
    return validation;
}

bool AssetValidator::detectCorruption(const std::string& mpqPath) {
    // Minimal implementation to check MPQ header
    std::ifstream file(mpqPath, std::ios::binary);
    if (!file) {
        return true; // File doesn't exist = corrupted
    }
    
    // Read first 4 bytes for MPQ header check
    char header[4];
    file.read(header, 4);
    
    // Check for MPQ header signature
    if (header[0] != 'M' || header[1] != 'P' || header[2] != 'Q' || header[3] != 0x1A) {
        return true; // Invalid header = corrupted
    }
    
    return false; // Valid header = not corrupted
}

bool AssetValidator::verifyChecksum(const std::string& filePath, const std::string& expectedChecksum) {
    // Will be implemented when we write tests for it
    return false;
}

} // namespace d2