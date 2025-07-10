#ifndef D2_ISO_EXTRACTOR_H
#define D2_ISO_EXTRACTOR_H

#include <string>
#include <vector>
#include <fstream>

namespace d2 {

/**
 * Information about a file in the ISO
 */
struct ISOFileInfo {
    bool exists = false;
    uint32_t size = 0;
    uint32_t sector = 0;
};

/**
 * ISO file extractor for extracting MPQ files from ISO images
 */
class ISOExtractor {
public:
    ISOExtractor() = default;
    ~ISOExtractor() {
        if (isoFile.is_open()) {
            isoFile.close();
        }
    }
    
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
    bool isOpen() const { return isOpenFlag; }
    
    /**
     * Close the currently open ISO file
     */
    void close();
    
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
     * Extract all files from the ISO to a directory
     * @param dest_dir Destination directory to extract files to
     * @return true if extraction successful, false otherwise
     */
    bool extractAll(const std::string& dest_dir);
    
    /**
     * Get information about a file in the ISO
     * @param filename Name of the file to get info for
     * @return ISOFileInfo struct with file information
     */
    ISOFileInfo getFileInfo(const std::string& filename) const;
    
    /**
     * Get the last error message
     * @return Error message string
     */
    std::string getLastError() const { return lastError; }
    
    /**
     * List all files in the ISO recursively (including files in subdirectories)
     * @return Vector of file paths found in the ISO with subdirectory paths
     */
    std::vector<std::string> listFilesRecursive() const;
    
private:
    std::string lastError;
    mutable std::ifstream isoFile;  // mutable for const listFiles()
    std::string isoPath;
    bool isOpenFlag = false;
    uint32_t rootDirSector = 0;
    uint32_t rootDirSize = 0;
};

} // namespace d2

#endif // D2_ISO_EXTRACTOR_H