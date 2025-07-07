#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdint>

namespace d2portable {
namespace utils {

/**
 * MockMPQBuilder - Creates mock MPQ files for testing purposes
 * 
 * This class uses StormLib to create minimal but valid MPQ archives
 * that can be used in integration tests without requiring real game files.
 */
class MockMPQBuilder {
public:
    enum class CompressionType {
        NONE,     // No compression
        ZLIB,     // Zlib compression
        PKWARE,   // PKWARE Data Compression Library
        BZIP2     // BZIP2 compression
    };
    
    MockMPQBuilder() = default;
    ~MockMPQBuilder() = default;
    
    /**
     * Add a file to the mock MPQ
     * @param filename The filename inside the MPQ (e.g., "data\\test\\file.txt")
     * @param data The file data
     */
    void addFile(const std::string& filename, const std::vector<uint8_t>& data);
    
    /**
     * Add a file to the mock MPQ with specific compression
     * @param filename The filename inside the MPQ (e.g., "data\\test\\file.txt")
     * @param data The file data
     * @param compression The compression algorithm to use
     */
    void addFileWithCompression(const std::string& filename, const std::vector<uint8_t>& data, CompressionType compression);
    
    /**
     * Build the MPQ file
     * @param outputPath Path where the MPQ file will be created
     * @return true if successful, false otherwise
     */
    bool build(const std::string& outputPath);
    
    /**
     * Clear all pending files
     */
    void clear();
    
    /**
     * Get information about compression algorithms used
     * @return Set of compression algorithm names used
     */
    std::set<std::string> getCompressionInfo() const;
    
private:
    struct FileEntry {
        std::string filename;
        std::vector<uint8_t> data;
        CompressionType compression;
    };
    
    std::vector<FileEntry> pending_files_;
    std::set<std::string> used_compression_types_;
};

} // namespace utils
} // namespace d2portable