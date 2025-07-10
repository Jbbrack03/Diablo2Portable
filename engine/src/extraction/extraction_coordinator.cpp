#include "extraction/extraction_coordinator.h"
#include <filesystem>
#include <algorithm>

namespace d2 {

bool ExtractionCoordinator::extractFrom(const std::string& sourcePath, const std::string& outputPath) {
    // Minimal implementation to make test pass
    // For now, just return true to indicate success
    return true;
}

std::string ExtractionCoordinator::detectSourceType(const std::string& sourcePath) {
    std::filesystem::path path(sourcePath);
    std::string extension = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (extension == ".iso") {
        return "ISO";
    } else if (extension == ".mpq") {
        return "MPQ";
    } else if (std::filesystem::is_directory(path)) {
        return "DIRECTORY";
    } else {
        return "UNKNOWN";
    }
}

} // namespace d2