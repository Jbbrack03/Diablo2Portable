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

std::vector<std::string> ISOExtractor::listFiles() const {
    // Return empty vector when not open
    return {};
}

bool ISOExtractor::extractFile(const std::string& source_path, const std::string& dest_path) {
    // Can't extract when not open
    if (!isOpen()) {
        lastError = "No ISO file is open";
        return false;
    }
    
    // Not implemented yet
    return false;
}

} // namespace d2