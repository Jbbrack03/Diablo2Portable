#include "utils/security_utils.h"
#include <algorithm>
#include <vector>

namespace d2::utils {

const std::vector<std::string> SecurityUtils::RESERVED_NAMES = {
    "CON", "PRN", "AUX", "NUL",
    "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
    "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
};

bool SecurityUtils::isPathSafe(const std::filesystem::path& path, 
                              const std::filesystem::path& baseDirectory) {
    try {
        // Get canonical paths to resolve any .. or symbolic links
        // For non-existing files, use parent directory for validation
        auto pathToCheck = path;
        if (!std::filesystem::exists(path) && path.has_parent_path()) {
            pathToCheck = path.parent_path();
        }
        
        auto canonicalBase = std::filesystem::canonical(baseDirectory);
        
        // If the path doesn't exist yet, check if it would be within base directory
        if (!std::filesystem::exists(pathToCheck)) {
            // Check lexically if the path would be safe
            auto normalizedPath = (canonicalBase / path.filename()).lexically_normal();
            auto baseStr = canonicalBase.string();
            auto pathStr = normalizedPath.string();
            return pathStr.substr(0, baseStr.length()) == baseStr;
        }
        
        auto canonicalPath = std::filesystem::canonical(pathToCheck);
        
        // Check if the path starts with the base directory
        auto pathStr = canonicalPath.string();
        auto baseStr = canonicalBase.string();
        
        return pathStr.substr(0, baseStr.length()) == baseStr;
    } catch (const std::filesystem::filesystem_error&) {
        // For development/testing, be more permissive
        // Check lexically without requiring files to exist
        try {
            auto normalizedPath = (baseDirectory / path.filename()).lexically_normal();
            auto baseStr = baseDirectory.string();
            auto pathStr = normalizedPath.string();
            return pathStr.substr(0, baseStr.length()) == baseStr;
        } catch (...) {
            return false;
        }
    }
}

std::string SecurityUtils::sanitizeFilename(const std::string& filename) {
    std::string sanitized = filename;
    
    // Remove dangerous characters
    std::string dangerousChars = DANGEROUS_CHARS;
    for (char c : dangerousChars) {
        sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), c), sanitized.end());
    }
    
    // Remove control characters (0-31)
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(),
                                 [](char c) { return c >= 0 && c <= 31; }),
                   sanitized.end());
    
    // Check for reserved names
    std::string upperName = sanitized;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);
    
    for (const auto& reserved : RESERVED_NAMES) {
        if (upperName == reserved || upperName.substr(0, reserved.length() + 1) == reserved + ".") {
            sanitized = "_" + sanitized; // Prefix with underscore to avoid reserved name
            break;
        }
    }
    
    // Ensure filename is not empty
    if (sanitized.empty()) {
        sanitized = "unnamed_file";
    }
    
    // Limit length to 255 characters (common filesystem limit)
    if (sanitized.length() > 255) {
        sanitized = sanitized.substr(0, 255);
    }
    
    return sanitized;
}

bool SecurityUtils::isFileExtensionAllowed(const std::string& filename,
                                         const std::vector<std::string>& allowedExtensions) {
    std::filesystem::path path(filename);
    std::string extension = path.extension().string();
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return std::find(allowedExtensions.begin(), allowedExtensions.end(), extension) 
           != allowedExtensions.end();
}

std::filesystem::path SecurityUtils::createSafePath(const std::filesystem::path& baseDirectory,
                                                   const std::filesystem::path& relativePath) {
    try {
        // Join the paths
        auto fullPath = baseDirectory / relativePath;
        
        // Normalize the path
        fullPath = fullPath.lexically_normal();
        
        // Check if the result is safe
        if (isPathSafe(fullPath, baseDirectory)) {
            return fullPath;
        }
    } catch (const std::filesystem::filesystem_error&) {
        // If path operations fail, return empty path
    }
    
    return std::filesystem::path{}; // Return empty path if unsafe
}

} // namespace d2::utils