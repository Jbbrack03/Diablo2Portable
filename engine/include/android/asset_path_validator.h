#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace d2 {

class AssetPathValidator {
public:
    struct ValidationResult {
        bool isValid = false;
        std::vector<std::string> missingFiles;
        std::vector<std::string> foundFiles;
        std::string errorMessage;
    };

    // Required MPQ files for the game to function
    static constexpr const char* REQUIRED_MPQS[] = {
        "d2data.mpq",
        "d2exp.mpq"
    };

    // Optional but recommended MPQ files
    static constexpr const char* OPTIONAL_MPQS[] = {
        "d2sfx.mpq",
        "d2music.mpq",
        "d2speech.mpq",
        "d2video.mpq",
        "d2char.mpq"
    };

    static ValidationResult validateAssetPath(const std::string& assetPath);
    static bool hasRequiredMPQFiles(const std::string& assetPath);
    static std::vector<std::string> findMPQFiles(const std::string& assetPath);
    
private:
    static bool isValidMPQFile(const std::filesystem::path& filePath);
};

} // namespace d2