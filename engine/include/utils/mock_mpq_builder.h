#pragma once

#include <string>
#include <vector>
#include <map>
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
    MockMPQBuilder() = default;
    ~MockMPQBuilder() = default;
    
    /**
     * Add a file to the mock MPQ
     * @param filename The filename inside the MPQ (e.g., "data\\test\\file.txt")
     * @param data The file data
     */
    void addFile(const std::string& filename, const std::vector<uint8_t>& data);
    
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
    
private:
    struct FileEntry {
        std::string filename;
        std::vector<uint8_t> data;
    };
    
    std::vector<FileEntry> pending_files_;
};

} // namespace utils
} // namespace d2portable