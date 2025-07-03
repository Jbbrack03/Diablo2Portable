#include "tools/asset_verifier.h"
#include <filesystem>
#include <vector>
#include <string>

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

} // namespace d2