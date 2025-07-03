#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace d2 {

// Forward declaration from file_source_detector.h
enum class D2Version;

struct AssetValidation {
    bool isComplete = false;
    std::vector<std::string> missingFiles;
    bool hasExpansion = false;
    D2Version version = static_cast<D2Version>(0); // Will be set to CLASSIC
    std::vector<std::string> corruptedFiles;
    int64_t totalSize = 0;
};

class AssetValidator {
public:
    AssetValidation validateAssets(const std::string& assetPath);
    bool detectCorruption(const std::string& mpqPath);
    bool verifyChecksum(const std::string& filePath, const std::string& expectedChecksum);
};

} // namespace d2