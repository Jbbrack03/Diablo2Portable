#include "tools/asset_verifier.h"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

namespace d2 {

VerificationResult AssetVerifier::fullVerification(const std::string& assetPath) {
    VerificationResult result;
    
    if (!fs::exists(assetPath)) {
        result.isComplete = false;
        return result;
    }
    
    // Define critical files that must exist
    std::vector<std::string> criticalFiles = {
        "data/global/ui/panel/invchar6.dc6",
        "data/global/chars/ba/cof/bacof.d2",
        "data/global/sfx/cursor/button.wav"
    };
    
    // Check for critical files
    for (const auto& file : criticalFiles) {
        fs::path fullPath = fs::path(assetPath) / file;
        if (!fs::exists(fullPath)) {
            result.missingCriticalFiles.push_back(file);
        }
    }
    
    // Validate all files in the directory
    for (const auto& entry : fs::recursive_directory_iterator(assetPath)) {
        if (entry.is_regular_file()) {
            // For now, just count files as validated
            // In a real implementation, we would check checksums
            result.validatedFiles++;
        }
    }
    
    // Mark as complete if no critical files are missing and we validated some files
    result.isComplete = result.missingCriticalFiles.empty() && result.validatedFiles > 0;
    
    return result;
}

// Simple checksum calculation (for testing)
static std::string calculateChecksum(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return "";
    }
    
    // Simple sum of bytes for now
    unsigned long checksum = 0;
    char byte;
    while (file.get(byte)) {
        checksum += static_cast<unsigned char>(byte);
    }
    
    std::stringstream ss;
    ss << std::hex << checksum;
    return ss.str();
}

ChecksumManifest AssetVerifier::generateChecksumManifest(const std::string& assetPath) {
    ChecksumManifest manifest;
    
    if (!fs::exists(assetPath)) {
        return manifest;
    }
    
    // Generate checksums for all files
    for (const auto& entry : fs::recursive_directory_iterator(assetPath)) {
        if (entry.is_regular_file()) {
            std::string relativePath = fs::relative(entry.path(), assetPath).string();
            std::string checksum = calculateChecksum(entry.path().string());
            
            if (!checksum.empty()) {
                manifest.fileChecksums[relativePath] = checksum;
                manifest.fileCount++;
            }
        }
    }
    
    // Generate manifest checksum (simple concatenation of all checksums)
    std::stringstream manifestData;
    for (const auto& [path, checksum] : manifest.fileChecksums) {
        manifestData << path << ":" << checksum << "\n";
    }
    
    // Calculate checksum of the manifest data itself
    unsigned long manifestChecksum = 0;
    std::string data = manifestData.str();
    for (char c : data) {
        manifestChecksum += static_cast<unsigned char>(c);
    }
    
    std::stringstream ss;
    ss << std::hex << manifestChecksum;
    manifest.manifestChecksum = ss.str();
    
    return manifest;
}

bool AssetVerifier::validateManifest(const ChecksumManifest& manifest) {
    // Check basic validity
    if (manifest.fileCount == 0 || manifest.manifestChecksum.empty()) {
        return false;
    }
    
    // Recalculate manifest checksum to verify integrity
    std::stringstream manifestData;
    for (const auto& [path, checksum] : manifest.fileChecksums) {
        manifestData << path << ":" << checksum << "\n";
    }
    
    // Calculate checksum of the manifest data
    unsigned long calculatedChecksum = 0;
    std::string data = manifestData.str();
    for (char c : data) {
        calculatedChecksum += static_cast<unsigned char>(c);
    }
    
    std::stringstream ss;
    ss << std::hex << calculatedChecksum;
    std::string expectedChecksum = ss.str();
    
    // Compare with stored checksum
    return manifest.manifestChecksum == expectedChecksum;
}

} // namespace d2