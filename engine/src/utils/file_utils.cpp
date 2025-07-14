#include "utils/file_utils.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstring>

namespace d2::utils {

thread_local std::string FileUtils::lastError_;

bool FileUtils::safeOpenBinaryFileForReading(const std::string& path, std::ifstream& file) {
    if (!validateFileExists(path)) {
        setLastError("File does not exist: " + path);
        return false;
    }
    
    file.open(path, std::ios::binary);
    if (!file.is_open()) {
        setLastError("Failed to open file for reading: " + path);
        return false;
    }
    
    return true;
}

bool FileUtils::safeOpenBinaryFileForWriting(const std::string& path, std::ofstream& file) {
    if (!createParentDirectories(path)) {
        setLastError("Failed to create parent directories for: " + path);
        return false;
    }
    
    file.open(path, std::ios::binary);
    if (!file.is_open()) {
        setLastError("Failed to open file for writing: " + path);
        return false;
    }
    
    return true;
}

bool FileUtils::safeOpenTextFileForReading(const std::string& path, std::ifstream& file) {
    if (!validateFileExists(path)) {
        setLastError("File does not exist: " + path);
        return false;
    }
    
    file.open(path);
    if (!file.is_open()) {
        setLastError("Failed to open file for reading: " + path);
        return false;
    }
    
    return true;
}

bool FileUtils::safeOpenTextFileForWriting(const std::string& path, std::ofstream& file) {
    if (!createParentDirectories(path)) {
        setLastError("Failed to create parent directories for: " + path);
        return false;
    }
    
    file.open(path);
    if (!file.is_open()) {
        setLastError("Failed to open file for writing: " + path);
        return false;
    }
    
    return true;
}

bool FileUtils::validateFileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool FileUtils::validateFileHeader(const std::string& path, const std::vector<uint8_t>& expectedHeader) {
    std::ifstream file;
    if (!safeOpenBinaryFileForReading(path, file)) {
        return false;
    }
    
    std::vector<uint8_t> actualHeader(expectedHeader.size());
    file.read(reinterpret_cast<char*>(actualHeader.data()), expectedHeader.size());
    
    if (!file.good()) {
        setLastError("Failed to read header from file: " + path);
        return false;
    }
    
    bool headerMatches = (actualHeader == expectedHeader);
    if (!headerMatches) {
        setLastError("File header does not match expected header: " + path);
    }
    
    return headerMatches;
}

bool FileUtils::validateFileHeader(const std::string& path, const std::string& expectedHeader) {
    std::vector<uint8_t> expectedBytes(expectedHeader.begin(), expectedHeader.end());
    return validateFileHeader(path, expectedBytes);
}

bool FileUtils::readEntireFile(const std::string& path, std::vector<uint8_t>& data) {
    std::ifstream file;
    if (!safeOpenBinaryFileForReading(path, file)) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    
    if (!file.good()) {
        setLastError("Failed to read entire file: " + path);
        return false;
    }
    
    return true;
}

bool FileUtils::readEntireFile(const std::string& path, std::string& data) {
    std::ifstream file;
    if (!safeOpenTextFileForReading(path, file)) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(fileSize);
    file.read(data.data(), fileSize);
    
    if (!file.good()) {
        setLastError("Failed to read entire file: " + path);
        return false;
    }
    
    return true;
}

bool FileUtils::writeEntireFile(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream file;
    if (!safeOpenBinaryFileForWriting(path, file)) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    if (!file.good()) {
        setLastError("Failed to write entire file: " + path);
        return false;
    }
    
    return true;
}

bool FileUtils::writeEntireFile(const std::string& path, const std::string& data) {
    std::ofstream file;
    if (!safeOpenTextFileForWriting(path, file)) {
        return false;
    }
    
    file.write(data.data(), data.size());
    
    if (!file.good()) {
        setLastError("Failed to write entire file: " + path);
        return false;
    }
    
    return true;
}

size_t FileUtils::getFileSize(const std::string& path) {
    if (!validateFileExists(path)) {
        return 0;
    }
    
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    if (ec) {
        setLastError("Failed to get file size: " + path + " - " + ec.message());
        return 0;
    }
    
    return size;
}

bool FileUtils::isFileEmpty(const std::string& path) {
    return getFileSize(path) == 0;
}

bool FileUtils::createParentDirectories(const std::string& path) {
    std::filesystem::path filePath(path);
    std::filesystem::path parentPath = filePath.parent_path();
    
    if (parentPath.empty()) {
        return true; // No parent directory needed
    }
    
    std::error_code ec;
    std::filesystem::create_directories(parentPath, ec);
    
    if (ec) {
        setLastError("Failed to create parent directories for: " + path + " - " + ec.message());
        return false;
    }
    
    return true;
}

bool FileUtils::safeCloseFile(std::ifstream& file, const std::string& path) {
    if (file.is_open()) {
        file.close();
        if (file.fail()) {
            setLastError("Failed to close file: " + path);
            return false;
        }
    }
    return true;
}

bool FileUtils::safeCloseFile(std::ofstream& file, const std::string& path) {
    if (file.is_open()) {
        file.close();
        if (file.fail()) {
            setLastError("Failed to close file: " + path);
            return false;
        }
    }
    return true;
}

std::string FileUtils::getLastError() {
    return lastError_;
}

void FileUtils::setLastError(const std::string& error) {
    lastError_ = error;
}

bool FileUtils::findMPQSignature(const std::string& path, size_t& mpqOffset) {
    std::ifstream file;
    if (!safeOpenBinaryFileForReading(path, file)) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    auto file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    const size_t buffer_size = 4096;
    std::vector<char> buffer(buffer_size);
    
    for (size_t offset = 0; offset < static_cast<size_t>(file_size); offset += buffer_size - 3) {
        file.seekg(offset);
        file.read(buffer.data(), buffer_size);
        auto bytes_read = file.gcount();
        
        size_t localOffset;
        if (findMPQSignature(buffer.data(), bytes_read, localOffset)) {
            mpqOffset = offset + localOffset;
            return true;
        }
    }
    
    return false;
}

bool FileUtils::findMPQSignature(const char* buffer, size_t bufferSize, size_t& mpqOffset) {
    if (bufferSize < 4) {
        return false;
    }
    
    // Search for MPQ signature: 'M' 'P' 'Q' 0x1A
    for (size_t i = 0; i < bufferSize - 3; ++i) {
        if (buffer[i] == 'M' && buffer[i+1] == 'P' && 
            buffer[i+2] == 'Q' && buffer[i+3] == 0x1A) {
            mpqOffset = i;
            return true;
        }
    }
    
    return false;
}

bool FileUtils::createDirectoriesFromList(const std::string& basePath, const std::vector<std::string>& relativePaths) {
    try {
        for (const auto& relativePath : relativePaths) {
            std::filesystem::path fullPath = std::filesystem::path(basePath) / relativePath;
            std::filesystem::create_directories(fullPath);
        }
        return true;
    } catch (const std::exception& e) {
        setLastError("Failed to create directories: " + std::string(e.what()));
        return false;
    }
}

} // namespace d2::utils