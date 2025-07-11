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

} // namespace d2::utils