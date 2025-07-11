#ifndef D2_UTILS_MPQ_VALIDATOR_H
#define D2_UTILS_MPQ_VALIDATOR_H

#include <string>
#include <vector>
#include <cstddef>

namespace d2 {
namespace utils {

/**
 * @brief Validates MPQ files to determine if they are authentic or placeholders
 * 
 * This validator helps identify which MPQ files in the vendor directory are
 * authentic game files versus placeholder files (filled with 'X' characters).
 * This is important for integration tests and asset extraction systems.
 */
class MPQValidator {
public:
    /**
     * @brief Result of MPQ file validation
     */
    struct ValidationResult {
        bool isValid = false;        ///< True if file has valid MPQ header
        bool isPlaceholder = false;  ///< True if file is filled with 'X' characters
        std::string error;           ///< Error message if validation failed
        size_t fileSize = 0;         ///< Size of the file in bytes
    };
    
    /**
     * @brief Validates a single MPQ file
     * @param filepath Path to the MPQ file to validate
     * @return ValidationResult containing validation status and details
     */
    static ValidationResult validateMPQFile(const std::string& filepath);
    
    /**
     * @brief Gets list of placeholder files from a list of file paths
     * @param files Vector of file paths to check
     * @return Vector of file paths that are placeholders
     */
    static std::vector<std::string> getPlaceholderFiles(const std::vector<std::string>& files);
    
    /**
     * @brief Gets list of authentic MPQ files from a list of file paths
     * @param files Vector of file paths to check
     * @return Vector of file paths that are authentic MPQ files
     */
    static std::vector<std::string> getAuthenticFiles(const std::vector<std::string>& files);
    
    /**
     * @brief Checks if a file is a known placeholder based on filename
     * @param filename The filename (not full path) to check
     * @return True if this is a known placeholder file
     */
    static bool isKnownPlaceholder(const std::string& filename);
    
private:
    // MPQ file header magic number
    static constexpr char MPQ_HEADER[4] = {'M', 'P', 'Q', 0x1A};
    
    // Placeholder pattern
    static constexpr char PLACEHOLDER_PATTERN[4] = {'X', 'X', 'X', 'X'};
    
    // Known placeholder files
    static const std::vector<std::string> KNOWN_PLACEHOLDERS;
};

} // namespace utils
} // namespace d2

#endif // D2_UTILS_MPQ_VALIDATOR_H