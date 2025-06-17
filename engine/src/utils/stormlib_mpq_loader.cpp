#include "utils/stormlib_mpq_loader.h"
#include <StormLib.h>
#include <cstring>
#include <algorithm>

namespace d2portable {
namespace utils {

class StormLibMPQLoader::Impl {
public:
    HANDLE hMpq = nullptr;
    std::string lastError;
    
    ~Impl() {
        if (hMpq) {
            SFileCloseArchive(hMpq);
        }
    }
    
    void setLastError() {
        DWORD error = GetLastError();
        switch (error) {
            case ERROR_FILE_NOT_FOUND:
                lastError = "File not found";
                break;
            case ERROR_ACCESS_DENIED:
                lastError = "Access denied";
                break;
            case ERROR_INVALID_HANDLE:
                lastError = "Invalid handle";
                break;
            case ERROR_NOT_ENOUGH_MEMORY:
                lastError = "Not enough memory";
                break;
            case ERROR_NOT_SUPPORTED:
                lastError = "Operation not supported";
                break;
            case ERROR_INVALID_PARAMETER:
                lastError = "Invalid parameter";
                break;
            case ERROR_FILE_CORRUPT:
                lastError = "File corrupt";
                break;
            default:
                lastError = "Error code: " + std::to_string(error);
                break;
        }
    }
};

StormLibMPQLoader::StormLibMPQLoader() : pImpl(std::make_unique<Impl>()) {
}

StormLibMPQLoader::~StormLibMPQLoader() = default;

StormLibMPQLoader::StormLibMPQLoader(StormLibMPQLoader&& other) noexcept = default;
StormLibMPQLoader& StormLibMPQLoader::operator=(StormLibMPQLoader&& other) noexcept = default;

bool StormLibMPQLoader::open(const std::string& filepath) {
    // Close any existing archive
    close();
    
    // Open the MPQ archive - use read-only mode for Diablo II MPQs
    if (!SFileOpenArchive(filepath.c_str(), 0, MPQ_OPEN_READ_ONLY, &pImpl->hMpq)) {
        pImpl->setLastError();
        return false;
    }
    
    return true;
}

void StormLibMPQLoader::close() {
    if (pImpl->hMpq) {
        SFileCloseArchive(pImpl->hMpq);
        pImpl->hMpq = nullptr;
    }
}

bool StormLibMPQLoader::isOpen() const {
    return pImpl->hMpq != nullptr;
}

std::vector<StormMPQFileInfo> StormLibMPQLoader::listFiles() const {
    std::vector<StormMPQFileInfo> result;
    
    if (!pImpl->hMpq) {
        return result;
    }
    
    // Find first file
    SFILE_FIND_DATA findData;
    HANDLE hFind = SFileFindFirstFile(pImpl->hMpq, "*", &findData, nullptr);
    
    if (hFind != nullptr) {
        do {
            StormMPQFileInfo info;
            info.filename = findData.cFileName;
            info.compressed_size = findData.dwCompSize;
            info.uncompressed_size = findData.dwFileSize;
            info.flags = findData.dwFileFlags;
            
            // Skip special files
            if (info.filename != "(listfile)" && 
                info.filename != "(signature)" &&
                info.filename != "(attributes)") {
                result.push_back(info);
            }
        } while (SFileFindNextFile(hFind, &findData));
        
        SFileFindClose(hFind);
    }
    
    return result;
}

bool StormLibMPQLoader::hasFile(const std::string& filename) const {
    if (!pImpl->hMpq) {
        return false;
    }
    
    return SFileHasFile(pImpl->hMpq, filename.c_str());
}

bool StormLibMPQLoader::extractFile(const std::string& filename, std::vector<uint8_t>& output) {
    if (!pImpl->hMpq) {
        pImpl->lastError = "MPQ not open";
        return false;
    }
    
    // Open file in MPQ
    HANDLE hFile;
    if (!SFileOpenFileEx(pImpl->hMpq, filename.c_str(), 0, &hFile)) {
        pImpl->setLastError();
        return false;
    }
    
    // Get file size
    DWORD fileSize = SFileGetFileSize(hFile, nullptr);
    if (fileSize == SFILE_INVALID_SIZE) {
        pImpl->setLastError();
        SFileCloseFile(hFile);
        return false;
    }
    
    // Allocate buffer
    output.resize(fileSize);
    
    // Read file data
    DWORD bytesRead = 0;
    if (!SFileReadFile(hFile, output.data(), fileSize, &bytesRead, nullptr)) {
        pImpl->setLastError();
        SFileCloseFile(hFile);
        return false;
    }
    
    // Close file
    SFileCloseFile(hFile);
    
    // Verify we read the full file
    if (bytesRead != fileSize) {
        pImpl->lastError = "Failed to read complete file";
        return false;
    }
    
    return true;
}

std::optional<StormMPQFileInfo> StormLibMPQLoader::getFileInfo(const std::string& filename) const {
    if (!pImpl->hMpq) {
        return std::nullopt;
    }
    
    // Open file to get info
    HANDLE hFile;
    if (!SFileOpenFileEx(pImpl->hMpq, filename.c_str(), 0, &hFile)) {
        return std::nullopt;
    }
    
    StormMPQFileInfo info;
    info.filename = filename;
    
    // Get file size
    info.uncompressed_size = SFileGetFileSize(hFile, nullptr);
    
    // Get file info for compressed size and flags
    SFileGetFileInfo(hFile, SFileInfoCompressedSize, &info.compressed_size, sizeof(DWORD), nullptr);
    SFileGetFileInfo(hFile, SFileInfoFlags, &info.flags, sizeof(DWORD), nullptr);
    
    SFileCloseFile(hFile);
    
    return info;
}

std::string StormLibMPQLoader::getLastError() const {
    return pImpl->lastError;
}

} // namespace utils
} // namespace d2portable