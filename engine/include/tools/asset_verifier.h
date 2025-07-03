#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace d2 {

/**
 * Result of asset verification
 */
struct VerificationResult {
    bool isComplete = false;
    std::vector<std::string> corruptedFiles;
    std::vector<std::string> missingCriticalFiles;
    int validatedFiles = 0;
    
    bool hasRequiredAssets() const {
        return missingCriticalFiles.empty();
    }
    
    bool canAttemptRepair() const {
        return !missingCriticalFiles.empty() || !corruptedFiles.empty();
    }
};

/**
 * Checksum manifest for asset distribution
 */
struct ChecksumManifest {
    int fileCount = 0;
    std::string manifestChecksum;
    std::unordered_map<std::string, std::string> fileChecksums;
};

/**
 * AssetVerifier - Validates extracted game assets
 * 
 * Features:
 * - Complete asset validation with checksums
 * - Missing file detection and reporting
 * - Corruption detection
 * - Critical file verification
 */
class AssetVerifier {
public:
    AssetVerifier() = default;
    ~AssetVerifier() = default;
    
    /**
     * Perform full verification of extracted assets
     * @param assetPath Path to the extracted assets directory
     * @return Verification result
     */
    VerificationResult fullVerification(const std::string& assetPath);
    
    /**
     * Generate checksum manifest for all assets
     * @param assetPath Path to the extracted assets directory
     * @return Generated manifest
     */
    ChecksumManifest generateChecksumManifest(const std::string& assetPath);
    
    /**
     * Validate a checksum manifest
     * @param manifest The manifest to validate
     * @return True if manifest is valid
     */
    bool validateManifest(const ChecksumManifest& manifest);
};

} // namespace d2