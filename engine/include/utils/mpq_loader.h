#ifndef D2PORTABLE_MPQ_LOADER_H
#define D2PORTABLE_MPQ_LOADER_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <optional>

namespace d2portable {
namespace utils {

/**
 * MPQ Archive file information
 */
struct MPQFileInfo {
    std::string filename;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint32_t flags;
    uint32_t locale;
    uint32_t platform;
};

/**
 * MPQ Archive reader for Diablo II game assets
 * 
 * This class provides functionality to read and extract files from
 * Blizzard's MPQ (Mo'PaQ) archive format used in Diablo II.
 */
class MPQLoader {
public:
    MPQLoader();
    ~MPQLoader();
    /**
     * Opens an MPQ archive file
     * @param filepath Path to the MPQ archive
     * @return true if successfully opened, false otherwise
     */
    bool open(const std::string& filepath);

    /**
     * Closes the currently open MPQ archive
     */
    void close();

    /**
     * Checks if an MPQ archive is currently open
     * @return true if an archive is open, false otherwise
     */
    bool isOpen() const;

    /**
     * Lists all files in the MPQ archive
     * @return Vector of file information structures
     */
    std::vector<MPQFileInfo> listFiles() const;

    /**
     * Checks if a file exists in the archive
     * @param filename Name of the file to check
     * @return true if file exists, false otherwise
     */
    bool hasFile(const std::string& filename) const;

    /**
     * Extracts a file from the archive
     * @param filename Name of the file to extract
     * @param output Vector to store the extracted file data
     * @return true if extraction successful, false otherwise
     */
    bool extractFile(const std::string& filename, std::vector<uint8_t>& output);

    /**
     * Gets information about a specific file
     * @param filename Name of the file
     * @return File information structure, or nullopt if file not found
     */
    std::optional<MPQFileInfo> getFileInfo(const std::string& filename) const;

    /**
     * Gets the last error message
     * @return Error message string
     */
    std::string getLastError() const;
    
    /**
     * Compute StormHash for a string (exposed for testing)
     * @param str String to hash
     * @param hash_type Hash type (0x000, 0x100, 0x200, 0x300)
     * @return Hash value
     */
    uint32_t hashString(const std::string& str, uint32_t hash_type) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace utils
} // namespace d2portable

#endif // D2PORTABLE_MPQ_LOADER_H