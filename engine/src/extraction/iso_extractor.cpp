#include "extraction/iso_extractor.h"
#include <filesystem>

namespace d2 {

bool ISOExtractor::open(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        lastError = "ISO file does not exist: " + filepath;
        return false;
    }
    
    // For now, just return false as we haven't implemented ISO reading yet
    lastError = "ISO reading not implemented yet";
    return false;
}

} // namespace d2