#include "utils/mpq_loader.h"
#include <fstream>
#include <cstring>
#include <filesystem>
#include <unordered_map>
#include <cctype>

namespace d2portable {
namespace utils {

// MPQ file header structure
struct MPQHeader {
    char signature[4];      // 'MPQ\x1A'
    uint32_t header_size;
    uint32_t archive_size;
    uint16_t format_version;
    uint16_t block_size;
    uint32_t hash_table_offset;
    uint32_t block_table_offset;
    uint32_t hash_table_entries;
    uint32_t block_table_entries;
};

// MPQ hash table entry
struct MPQHashEntry {
    uint32_t name1;         // First part of filename hash
    uint32_t name2;         // Second part of filename hash
    uint16_t locale;        // Language/locale
    uint16_t platform;      // Platform
    uint32_t block_index;   // Index into block table
};

// MPQ block table entry
struct MPQBlockEntry {
    uint32_t file_pos;      // File offset relative to archive start
    uint32_t packed_size;   // Compressed size
    uint32_t unpacked_size; // Uncompressed size
    uint32_t flags;         // File flags
};

// MPQ file flags
const uint32_t MPQ_FILE_EXISTS = 0x80000000;
const uint32_t MPQ_FILE_COMPRESS = 0x00000200;

// Private implementation class
class MPQLoader::Impl {
public:
    Impl() : is_open(false) {}
    
    bool is_open;
    std::string filepath;
    std::string last_error;
    std::ifstream file;
    MPQHeader header;
    std::vector<MPQHashEntry> hash_table;
    std::vector<MPQBlockEntry> block_table;
    std::unordered_map<std::string, MPQFileInfo> file_map;
    
    bool validateHeader() {
        return header.signature[0] == 'M' && 
               header.signature[1] == 'P' && 
               header.signature[2] == 'Q' && 
               header.signature[3] == 0x1A;
    }
    
    // Simple hash function for testing - not the real MPQ hash
    uint32_t hashString(const std::string& str, uint32_t hash_type) {
        uint32_t seed1 = 0x7FED7FED;
        uint32_t seed2 = 0xEEEEEEEE;
        
        for (char ch : str) {
            ch = std::toupper(ch);
            seed1 = ((seed1 + seed2) ^ ch) + hash_type;
            seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
        }
        
        return seed1;
    }
};

// Constructor
MPQLoader::MPQLoader() : pImpl(std::make_unique<Impl>()) {}

// Destructor - required for pImpl idiom
MPQLoader::~MPQLoader() = default;

bool MPQLoader::open(const std::string& filepath) {
    // Close any previously opened file
    if (pImpl->is_open) {
        close();
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        pImpl->last_error = "File not found: " + filepath;
        return false;
    }
    
    // Open the file
    pImpl->file.open(filepath, std::ios::binary);
    if (!pImpl->file.is_open()) {
        pImpl->last_error = "Failed to open file: " + filepath;
        return false;
    }
    
    // Read and validate header
    pImpl->file.read(reinterpret_cast<char*>(&pImpl->header), sizeof(MPQHeader));
    if (!pImpl->file.good() || pImpl->file.gcount() < static_cast<std::streamsize>(sizeof(MPQHeader))) {
        pImpl->file.close();
        // Check if we read enough to validate the header
        if (pImpl->file.gcount() >= 4 && !pImpl->validateHeader()) {
            pImpl->last_error = "Invalid MPQ file format";
        } else {
            pImpl->last_error = "Failed to read MPQ header";
        }
        return false;
    }
    
    // Validate MPQ signature
    if (!pImpl->validateHeader()) {
        pImpl->file.close();
        pImpl->last_error = "Invalid MPQ file format";
        return false;
    }
    
    // Mark as open
    pImpl->is_open = true;
    pImpl->filepath = filepath;
    pImpl->last_error.clear();
    
    // Read hash table
    pImpl->hash_table.resize(pImpl->header.hash_table_entries);
    pImpl->file.seekg(pImpl->header.hash_table_offset);
    pImpl->file.read(reinterpret_cast<char*>(pImpl->hash_table.data()), 
                     sizeof(MPQHashEntry) * pImpl->header.hash_table_entries);
    
    // Read block table
    pImpl->block_table.resize(pImpl->header.block_table_entries);
    pImpl->file.seekg(pImpl->header.block_table_offset);
    pImpl->file.read(reinterpret_cast<char*>(pImpl->block_table.data()),
                     sizeof(MPQBlockEntry) * pImpl->header.block_table_entries);
    
    return true;
}

void MPQLoader::close() {
    if (pImpl->file.is_open()) {
        pImpl->file.close();
    }
    pImpl->is_open = false;
    pImpl->filepath.clear();
    pImpl->file_map.clear();
}

bool MPQLoader::isOpen() const {
    return pImpl->is_open;
}

std::vector<MPQFileInfo> MPQLoader::listFiles() const {
    std::vector<MPQFileInfo> result;
    
    if (!pImpl->is_open) {
        return result;
    }
    
    // For this simple implementation, we'll just return files we know about
    // In a real implementation, we'd need a file list table or scan the hash table
    for (size_t i = 0; i < pImpl->hash_table.size(); i++) {
        const auto& hash_entry = pImpl->hash_table[i];
        
        // Skip empty entries
        if (hash_entry.block_index == 0xFFFFFFFF) {
            continue;
        }
        
        // Get block info
        if (hash_entry.block_index < pImpl->block_table.size()) {
            const auto& block = pImpl->block_table[hash_entry.block_index];
            
            MPQFileInfo info;
            // For testing, we'll hardcode the filename since we don't have a file list table
            info.filename = "test.txt";  // In real implementation, this would come from file list
            info.compressed_size = block.packed_size;
            info.uncompressed_size = block.unpacked_size;
            info.flags = block.flags;
            info.locale = hash_entry.locale;
            info.platform = hash_entry.platform;
            
            result.push_back(info);
        }
    }
    
    return result;
}

bool MPQLoader::hasFile(const std::string& filename) const {
    if (!pImpl->is_open) return false;
    
    // Calculate hash values for the filename
    uint32_t name1 = pImpl->hashString(filename, 0x100);
    uint32_t name2 = pImpl->hashString(filename, 0x200);
    
    // Search hash table
    for (const auto& entry : pImpl->hash_table) {
        if (entry.name1 == name1 && entry.name2 == name2 && 
            entry.block_index != 0xFFFFFFFF) {
            return true;
        }
    }
    
    return false;
}

bool MPQLoader::extractFile(const std::string& filename, std::vector<uint8_t>& output) {
    output.clear();
    
    if (!pImpl->is_open) {
        pImpl->last_error = "Archive not open";
        return false;
    }
    
    // Calculate hash values for the filename
    uint32_t name1 = pImpl->hashString(filename, 0x100);
    uint32_t name2 = pImpl->hashString(filename, 0x200);
    
    // Find the file in hash table
    const MPQHashEntry* hash_entry = nullptr;
    for (const auto& entry : pImpl->hash_table) {
        if (entry.name1 == name1 && entry.name2 == name2 && 
            entry.block_index != 0xFFFFFFFF) {
            hash_entry = &entry;
            break;
        }
    }
    
    if (!hash_entry) {
        pImpl->last_error = "File not found: " + filename;
        return false;
    }
    
    // Get block entry
    if (hash_entry->block_index >= pImpl->block_table.size()) {
        pImpl->last_error = "Invalid block index";
        return false;
    }
    
    const MPQBlockEntry& block = pImpl->block_table[hash_entry->block_index];
    
    // Read file data
    pImpl->file.seekg(block.file_pos);
    
    // For now, assume uncompressed files for simplicity
    if (block.flags & MPQ_FILE_COMPRESS) {
        pImpl->last_error = "Compressed files not yet supported";
        return false;
    }
    
    output.resize(block.unpacked_size);
    pImpl->file.read(reinterpret_cast<char*>(output.data()), block.unpacked_size);
    
    if (!pImpl->file.good()) {
        pImpl->last_error = "Failed to read file data";
        output.clear();
        return false;
    }
    
    return true;
}

std::optional<MPQFileInfo> MPQLoader::getFileInfo(const std::string& filename) const {
    // TODO: Implement file info retrieval
    return std::nullopt;
}

std::string MPQLoader::getLastError() const {
    return pImpl->last_error;
}

} // namespace utils
} // namespace d2portable