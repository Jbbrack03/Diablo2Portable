#include "extraction/iso_extractor.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstring>

namespace d2 {

bool ISOExtractor::open(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        lastError = "ISO file does not exist: " + filepath;
        return false;
    }
    
    // Open the ISO file
    isoFile.open(filepath, std::ios::binary);
    if (!isoFile.is_open()) {
        lastError = "Failed to open ISO file: " + filepath;
        return false;
    }
    
    // Read and validate Primary Volume Descriptor
    // Seek to sector 16 (skip system area)
    isoFile.seekg(16 * 2048);
    
    std::vector<uint8_t> pvd(2048);
    isoFile.read(reinterpret_cast<char*>(pvd.data()), pvd.size());
    
    if (!isoFile.good()) {
        lastError = "Failed to read Primary Volume Descriptor";
        isoFile.close();
        return false;
    }
    
    // Validate PVD
    if (pvd[0] != 0x01) { // Type code for PVD
        lastError = "Invalid Primary Volume Descriptor type";
        isoFile.close();
        return false;
    }
    
    if (std::memcmp(pvd.data() + 1, "CD001", 5) != 0) {
        lastError = "Invalid ISO 9660 signature";
        isoFile.close();
        return false;
    }
    
    if (pvd[6] != 0x01) { // Version
        lastError = "Unsupported ISO 9660 version";
        isoFile.close();
        return false;
    }
    
    // Extract root directory location from PVD
    // Root directory record is at offset 156 in PVD
    // Location of extent (LBA) is at offset 2 in directory record
    rootDirSector = pvd[156 + 2] | (pvd[156 + 3] << 8) | 
                    (pvd[156 + 4] << 16) | (pvd[156 + 5] << 24);
    rootDirSize = pvd[156 + 10] | (pvd[156 + 11] << 8) | 
                  (pvd[156 + 12] << 16) | (pvd[156 + 13] << 24);
    
    isoPath = filepath;
    isOpenFlag = true;
    lastError.clear();
    return true;
}

std::vector<std::string> ISOExtractor::listFiles() const {
    // Return empty vector when not open
    if (!isOpen()) {
        return {};
    }
    
    std::vector<std::string> files;
    
    // Seek to root directory
    isoFile.seekg(rootDirSector * 2048);
    
    // Read the root directory
    std::vector<uint8_t> dirData(rootDirSize);
    isoFile.read(reinterpret_cast<char*>(dirData.data()), dirData.size());
    
    if (!isoFile.good()) {
        return {};
    }
    
    // Parse directory entries
    size_t offset = 0;
    while (offset < dirData.size()) {
        uint8_t recordLength = dirData[offset];
        
        // End of directory entries
        if (recordLength == 0) {
            break;
        }
        
        // Skip if we'd go past the end
        if (offset + recordLength > dirData.size()) {
            break;
        }
        
        // Extract file flags (offset 25)
        uint8_t flags = dirData[offset + 25];
        
        // Skip directories (flag 0x02)
        if (!(flags & 0x02)) {
            // Extract identifier length (offset 32)
            uint8_t identLength = dirData[offset + 32];
            
            // Extract identifier (offset 33)
            if (identLength > 0) {
                std::string filename(reinterpret_cast<char*>(&dirData[offset + 33]), identLength);
                
                // ISO 9660 Level 1 uses ";1" version suffix, remove it
                size_t semicolon = filename.find(';');
                if (semicolon != std::string::npos) {
                    filename = filename.substr(0, semicolon);
                }
                
                files.push_back(filename);
            }
        }
        
        offset += recordLength;
    }
    
    return files;
}

bool ISOExtractor::extractFile(const std::string& source_path, const std::string& dest_path) {
    // Can't extract when not open
    if (!isOpen()) {
        lastError = "No ISO file is open";
        return false;
    }
    
    // First, find the file in the directory
    // Seek to root directory
    isoFile.seekg(rootDirSector * 2048);
    
    // Read the root directory
    std::vector<uint8_t> dirData(rootDirSize);
    isoFile.read(reinterpret_cast<char*>(dirData.data()), dirData.size());
    
    if (!isoFile.good()) {
        lastError = "Failed to read root directory";
        return false;
    }
    
    // Parse directory entries to find the file
    size_t offset = 0;
    bool found = false;
    uint32_t fileSector = 0;
    uint32_t fileSize = 0;
    
    while (offset < dirData.size() && !found) {
        uint8_t recordLength = dirData[offset];
        
        // End of directory entries
        if (recordLength == 0) {
            break;
        }
        
        // Skip if we'd go past the end
        if (offset + recordLength > dirData.size()) {
            break;
        }
        
        // Extract file flags (offset 25)
        uint8_t flags = dirData[offset + 25];
        
        // Skip directories (flag 0x02)
        if (!(flags & 0x02)) {
            // Extract identifier length (offset 32)
            uint8_t identLength = dirData[offset + 32];
            
            // Extract identifier (offset 33)
            if (identLength > 0) {
                std::string filename(reinterpret_cast<char*>(&dirData[offset + 33]), identLength);
                
                // ISO 9660 Level 1 uses ";1" version suffix, remove it
                size_t semicolon = filename.find(';');
                if (semicolon != std::string::npos) {
                    filename = filename.substr(0, semicolon);
                }
                
                if (filename == source_path) {
                    // Found the file! Extract location and size
                    fileSector = dirData[offset + 2] | (dirData[offset + 3] << 8) | 
                                (dirData[offset + 4] << 16) | (dirData[offset + 5] << 24);
                    fileSize = dirData[offset + 10] | (dirData[offset + 11] << 8) | 
                              (dirData[offset + 12] << 16) | (dirData[offset + 13] << 24);
                    found = true;
                }
            }
        }
        
        offset += recordLength;
    }
    
    if (!found) {
        lastError = "File not found in ISO: " + source_path;
        return false;
    }
    
    // Now extract the file
    isoFile.seekg(fileSector * 2048);
    
    std::vector<uint8_t> fileData(fileSize);
    isoFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    
    if (!isoFile.good()) {
        lastError = "Failed to read file data from ISO";
        return false;
    }
    
    // Write to destination
    std::ofstream outFile(dest_path, std::ios::binary);
    if (!outFile.is_open()) {
        lastError = "Failed to create output file: " + dest_path;
        return false;
    }
    
    outFile.write(reinterpret_cast<char*>(fileData.data()), fileSize);
    outFile.close();
    
    if (!outFile.good()) {
        lastError = "Failed to write output file: " + dest_path;
        return false;
    }
    
    return true;
}

void ISOExtractor::close() {
    if (isoFile.is_open()) {
        isoFile.close();
    }
    isOpenFlag = false;
    isoPath.clear();
    rootDirSector = 0;
    rootDirSize = 0;
}

} // namespace d2