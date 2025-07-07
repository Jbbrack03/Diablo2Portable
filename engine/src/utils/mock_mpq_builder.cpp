#include "utils/mock_mpq_builder.h"
#include <StormLib.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace d2portable {
namespace utils {

void MockMPQBuilder::addFile(const std::string& filename, const std::vector<uint8_t>& data) {
    pending_files_.push_back({filename, data, CompressionType::ZLIB});
    used_compression_types_.insert("ZLIB");
}

void MockMPQBuilder::addFileWithCompression(const std::string& filename, const std::vector<uint8_t>& data, CompressionType compression) {
    pending_files_.push_back({filename, data, compression});
    
    // Record the compression type used
    switch (compression) {
        case CompressionType::NONE:
            used_compression_types_.insert("NONE");
            break;
        case CompressionType::ZLIB:
            used_compression_types_.insert("ZLIB");
            break;
        case CompressionType::PKWARE:
            used_compression_types_.insert("PKWARE");
            break;
        case CompressionType::BZIP2:
            used_compression_types_.insert("BZIP2");
            break;
    }
}

bool MockMPQBuilder::build(const std::string& outputPath) {
    // Create parent directory if needed
    std::filesystem::path path(outputPath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    // Create the MPQ archive
    HANDLE hMpq = nullptr;
    
    // Use MPQ version 1 for compatibility
    constexpr DWORD dwCreateFlags = MPQ_CREATE_ARCHIVE_V1 | MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES;
    constexpr DWORD dwMaxFileCount = 1024; // Should be enough for testing
    
    if (!SFileCreateArchive(outputPath.c_str(), dwCreateFlags, dwMaxFileCount, &hMpq)) {
        std::cerr << "Failed to create MPQ archive: " << outputPath << std::endl;
        return false;
    }
    
    // Add each file to the archive
    for (const auto& entry : pending_files_) {
        // Create a temporary file with the data
        std::filesystem::path tempPath = std::filesystem::temp_directory_path() / "mpq_temp_file.dat";
        {
            std::ofstream tempFile(tempPath, std::ios::binary);
            if (!tempFile) {
                std::cerr << "Failed to create temp file" << std::endl;
                SFileCloseArchive(hMpq);
                return false;
            }
            tempFile.write(reinterpret_cast<const char*>(entry.data.data()), entry.data.size());
        }
        
        // Add the file to the MPQ with specified compression
        DWORD dwFlags = MPQ_FILE_REPLACEEXISTING;
        DWORD dwCompression = MPQ_COMPRESSION_ZLIB;
        
        // Set compression based on file entry
        switch (entry.compression) {
            case CompressionType::NONE:
                // No compression flag
                break;
            case CompressionType::ZLIB:
                dwFlags |= MPQ_FILE_COMPRESS;
                dwCompression = MPQ_COMPRESSION_ZLIB;
                break;
            case CompressionType::PKWARE:
                dwFlags |= MPQ_FILE_COMPRESS;
                dwCompression = MPQ_COMPRESSION_PKWARE;
                break;
            case CompressionType::BZIP2:
                dwFlags |= MPQ_FILE_COMPRESS;
                dwCompression = MPQ_COMPRESSION_BZIP2;
                break;
        }
        
        if (!SFileAddFileEx(hMpq, tempPath.c_str(), entry.filename.c_str(), 
                           dwFlags, dwCompression, MPQ_COMPRESSION_NEXT_SAME)) {
            std::cerr << "Failed to add file to MPQ: " << entry.filename << std::endl;
            std::filesystem::remove(tempPath);
            SFileCloseArchive(hMpq);
            return false;
        }
        
        // Clean up temp file
        std::filesystem::remove(tempPath);
    }
    
    // Finalize the archive
    if (!SFileFlushArchive(hMpq)) {
        std::cerr << "Failed to flush MPQ archive" << std::endl;
        SFileCloseArchive(hMpq);
        return false;
    }
    
    // Close the archive
    if (!SFileCloseArchive(hMpq)) {
        std::cerr << "Failed to close MPQ archive" << std::endl;
        return false;
    }
    
    return true;
}

void MockMPQBuilder::clear() {
    pending_files_.clear();
    used_compression_types_.clear();
}

std::set<std::string> MockMPQBuilder::getCompressionInfo() const {
    return used_compression_types_;
}

} // namespace utils
} // namespace d2portable