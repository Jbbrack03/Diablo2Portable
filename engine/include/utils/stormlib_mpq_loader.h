#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace d2portable {
namespace utils {

/**
 * MPQ file information structure
 */
struct StormMPQFileInfo {
    std::string filename;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint32_t flags;
};

/**
 * StormLib-based MPQ archive loader
 * 
 * This class provides a wrapper around StormLib for loading and extracting
 * files from MPQ archives, specifically for Diablo II game assets.
 */
class StormLibMPQLoader {
public:
    StormLibMPQLoader();
    ~StormLibMPQLoader();

    // Disable copy operations
    StormLibMPQLoader(const StormLibMPQLoader&) = delete;
    StormLibMPQLoader& operator=(const StormLibMPQLoader&) = delete;

    // Enable move operations
    StormLibMPQLoader(StormLibMPQLoader&& other) noexcept;
    StormLibMPQLoader& operator=(StormLibMPQLoader&& other) noexcept;

    /**
     * Opens an MPQ archive
     * @param filepath Path to the MPQ file
     * @return true if successful, false otherwise
     */
    bool open(const std::string& filepath);

    /**
     * Closes the current MPQ archive
     */
    void close();

    /**
     * Checks if an MPQ archive is currently open
     * @return true if open, false otherwise
     */
    bool isOpen() const;

    /**
     * Lists all files in the MPQ archive
     * @return Vector of file information structures
     */
    std::vector<StormMPQFileInfo> listFiles() const;

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
    std::optional<StormMPQFileInfo> getFileInfo(const std::string& filename) const;

    /**
     * Gets the last error message
     * @return Error message string
     */
    std::string getLastError() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace utils
} // namespace d2portable