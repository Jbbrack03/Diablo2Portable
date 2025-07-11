#include "utils/mpq_validator.h"
#include <fstream>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

namespace d2 {
namespace utils {

// Define known placeholder files
const std::vector<std::string> MPQValidator::KNOWN_PLACEHOLDERS = {
    "d2data.mpq",
    "d2exp.mpq",
    "d2sfx.mpq"
};

MPQValidator::ValidationResult MPQValidator::validateMPQFile(const std::string& filepath) {
    ValidationResult result;
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        result.error = "Failed to open file";
        return result;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    result.fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read first 4 bytes
    char header[4];
    file.read(header, 4);
    
    if (!file.good()) {
        result.error = "Failed to read file header";
        return result;
    }
    
    // Check for placeholder pattern (XXXX)
    if (std::memcmp(header, PLACEHOLDER_PATTERN, 4) == 0) {
        result.isPlaceholder = true;
        result.isValid = false;
        result.error = "File is a placeholder (filled with 'X' characters)";
        return result;
    }
    
    // Check for valid MPQ header
    if (std::memcmp(header, MPQ_HEADER, 4) == 0) {
        result.isValid = true;
        return result;
    }
    
    result.error = "Invalid MPQ header";
    return result;
}

std::vector<std::string> MPQValidator::getPlaceholderFiles(const std::vector<std::string>& files) {
    std::vector<std::string> placeholders;
    for (const auto& file : files) {
        auto result = validateMPQFile(file);
        if (result.isPlaceholder) {
            placeholders.push_back(file);
        }
    }
    return placeholders;
}

std::vector<std::string> MPQValidator::getAuthenticFiles(const std::vector<std::string>& files) {
    std::vector<std::string> authentic;
    for (const auto& file : files) {
        auto result = validateMPQFile(file);
        if (result.isValid && !result.isPlaceholder) {
            authentic.push_back(file);
        }
    }
    return authentic;
}

bool MPQValidator::isKnownPlaceholder(const std::string& filename) {
    // Extract just the filename from the path
    fs::path path(filename);
    std::string name = path.filename().string();
    
    // Check if it's in our known placeholder list
    for (const auto& placeholder : KNOWN_PLACEHOLDERS) {
        if (name == placeholder) {
            return true;
        }
    }
    return false;
}

} // namespace utils
} // namespace d2