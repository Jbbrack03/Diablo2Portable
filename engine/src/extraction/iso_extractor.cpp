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
    
    isoPath = filepath;
    isOpenFlag = true;
    lastError.clear();
    return true;
}

std::vector<std::string> ISOExtractor::listFiles() const {
    // Return empty vector when not open
    return {};
}

bool ISOExtractor::extractFile(const std::string& source_path, const std::string& dest_path) {
    // Can't extract when not open
    if (!isOpen()) {
        lastError = "No ISO file is open";
        return false;
    }
    
    // Not implemented yet
    return false;
}

} // namespace d2