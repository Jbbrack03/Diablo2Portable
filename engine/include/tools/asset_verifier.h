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
};

} // namespace d2