#pragma once

#include <string>
#include <filesystem>

namespace d2::utils {

/**
 * Security utilities for safe file and path operations
 */
class SecurityUtils {
public:
    /**
     * Validates that a path is safe and doesn't contain path traversal attempts
     * @param path The path to validate
     * @param baseDirectory The base directory that the path should be contained within
     * @return true if the path is safe, false otherwise
     */
    static bool isPathSafe(const std::filesystem::path& path, 
                          const std::filesystem::path& baseDirectory);
    
    /**
     * Sanitizes a filename by removing potentially dangerous characters
     * @param filename The filename to sanitize
     * @return Sanitized filename
     */
    static std::string sanitizeFilename(const std::string& filename);
    
    /**
     * Validates that a file extension is allowed
     * @param filename The filename to check
     * @param allowedExtensions List of allowed extensions (e.g., {".mpq", ".d2s"})
     * @return true if extension is allowed, false otherwise
     */
    static bool isFileExtensionAllowed(const std::string& filename,
                                     const std::vector<std::string>& allowedExtensions);
    
    /**
     * Creates a safe path by joining base directory with a relative path
     * Ensures the result stays within the base directory
     * @param baseDirectory The base directory
     * @param relativePath The relative path to join
     * @return Safe absolute path, or empty path if unsafe
     */
    static std::filesystem::path createSafePath(const std::filesystem::path& baseDirectory,
                                              const std::filesystem::path& relativePath);

private:
    // List of dangerous filename characters
    static constexpr const char* DANGEROUS_CHARS = "<>:\"|?*";
    
    // List of reserved Windows filenames
    static const std::vector<std::string> RESERVED_NAMES;
};

} // namespace d2::utils