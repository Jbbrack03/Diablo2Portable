#ifndef D2_ISO_EXTRACTOR_H
#define D2_ISO_EXTRACTOR_H

#include <string>
#include <vector>

namespace d2 {

/**
 * ISO file extractor for extracting MPQ files from ISO images
 */
class ISOExtractor {
public:
    ISOExtractor() = default;
    ~ISOExtractor() = default;
    
    /**
     * Open an ISO file for extraction
     * @param filepath Path to the ISO file
     * @return true if opened successfully, false otherwise
     */
    bool open(const std::string& filepath);
    
    /**
     * Check if an ISO file is currently open
     * @return true if ISO is open, false otherwise
     */
    bool isOpen() const { return false; }
    
    /**
     * List all files in the ISO
     * @return Vector of file paths found in the ISO
     */
    std::vector<std::string> listFiles() const;
    
    /**
     * Extract a file from the ISO
     * @param source_path Path to file within the ISO
     * @param dest_path Destination path to save the extracted file
     * @return true if extraction successful, false otherwise
     */
    bool extractFile(const std::string& source_path, const std::string& dest_path);
    
    /**
     * Get the last error message
     * @return Error message string
     */
    std::string getLastError() const { return lastError; }
    
private:
    std::string lastError;
};

} // namespace d2

#endif // D2_ISO_EXTRACTOR_H