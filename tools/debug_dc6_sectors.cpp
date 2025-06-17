#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2portable::utils;

// Define MPQ constants we need
const uint32_t MPQ_FILE_COMPRESSED = 0x00000200;
const uint32_t MPQ_FILE_ENCRYPTED = 0x00010000;
const uint32_t MPQ_FILE_SINGLE_UNIT = 0x01000000;

class MPQDebugLoader : public MPQLoader {
public:
    bool debugExtractFile(const std::string& filename) {
        // Get file info
        auto files = listFiles();
        FileInfo file_info;
        bool found = false;
        
        for (const auto& info : files) {
            if (info.filename == filename) {
                file_info = info;
                found = true;
                break;
            }
        }
        
        if (!found && !hasFile(filename)) {
            std::cout << "File not found: " << filename << "\n";
            return false;
        }
        
        // If not found in list but hasFile works, use hash lookup
        if (!found) {
            // Get block index from hash
            uint32_t hash_a = hashString(filename, 0x100);
            uint32_t hash_b = hashString(filename, 0x200);
            
            for (const auto& entry : mpq.hash_table) {
                if (entry.file_hash_a == hash_a && entry.file_hash_b == hash_b) {
                    uint32_t block_index = entry.block_index;
                    if (block_index < mpq.block_table.size()) {
                        auto& block = mpq.block_table[block_index];
                        file_info.compressed_size = block.compressed_size;
                        file_info.uncompressed_size = block.uncompressed_size;
                        file_info.flags = block.flags;
                        file_info.filename = filename;
                        found = true;
                        break;
                    }
                }
            }
        }
        
        if (!found) {
            std::cout << "Failed to get file info\n";
            return false;
        }
        
        std::cout << "\nFile: " << filename << "\n";
        std::cout << "Compressed size: " << file_info.compressed_size << "\n";
        std::cout << "Uncompressed size: " << file_info.uncompressed_size << "\n";
        std::cout << "Flags: 0x" << std::hex << file_info.flags << std::dec << "\n";
        
        bool compressed = file_info.flags & MPQ_FILE_COMPRESSED;
        bool encrypted = file_info.flags & MPQ_FILE_ENCRYPTED;
        bool single_unit = file_info.flags & MPQ_FILE_SINGLE_UNIT;
        
        std::cout << "Compressed: " << (compressed ? "YES" : "NO") << "\n";
        std::cout << "Encrypted: " << (encrypted ? "YES" : "NO") << "\n";
        std::cout << "Single unit: " << (single_unit ? "YES" : "NO") << "\n";
        
        if (!single_unit) {
            std::cout << "\nSector-based compression detected\n";
            
            // Calculate number of sectors
            uint32_t sector_size = 0x1000; // 4KB sectors
            uint32_t num_sectors = (file_info.uncompressed_size + sector_size - 1) / sector_size;
            std::cout << "Number of sectors: " << num_sectors << "\n";
            
            // Try to read raw file data to examine compression masks
            // This would require adding a method to MPQLoader to get raw block data
        }
        
        // Try normal extraction to see where it fails
        std::cout << "\nAttempting extraction...\n";
        std::vector<uint8_t> data;
        if (extractFile(filename, data)) {
            std::cout << "✅ Extraction successful! Got " << data.size() << " bytes\n";
            return true;
        } else {
            std::cout << "❌ Extraction failed: " << getLastError() << "\n";
            return false;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_d2data.mpq>\n";
        return 1;
    }
    
    MPQDebugLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "=== DC6 Sector Debug ===\n";
    
    // Test specific DC6 files
    std::vector<std::string> dc6_files = {
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\local\\font\\latin\\font8.dc6"
    };
    
    for (const auto& file : dc6_files) {
        loader.debugExtractFile(file);
    }
    
    return 0;
}