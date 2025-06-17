#include <iostream>
#include <vector>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2portable::utils;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_d2data.mpq>\n";
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "=== DC6 File Compression Debug ===\n\n";
    
    // Test specific DC6 files
    std::vector<std::string> dc6_files = {
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\local\\font\\latin\\font8.dc6",
        "data\\local\\font\\latin\\font16.dc6"
    };
    
    // Get raw file info from MPQ
    auto& mpq = loader.getMPQ();
    
    for (const auto& filename : dc6_files) {
        std::cout << "File: " << filename << "\n";
        
        // Try to get the file info directly
        uint32_t file_index = 0;
        bool found = false;
        
        // Look through all files
        auto all_files = loader.listFiles();
        for (size_t i = 0; i < all_files.size(); i++) {
            if (all_files[i].filename == filename) {
                file_index = i;
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::cout << "  Not found in MPQ\n\n";
            continue;
        }
        
        // Get the file's block entry
        auto& block_table = mpq.block_table;
        if (file_index < block_table.size()) {
            auto& block = block_table[file_index];
            
            std::cout << "  Block info:\n";
            std::cout << "    Flags: 0x" << std::hex << block.flags << std::dec << "\n";
            std::cout << "    Compressed size: " << block.compressed_size << "\n";
            std::cout << "    Uncompressed size: " << block.uncompressed_size << "\n";
            
            // Check compression flags
            if (block.flags & 0x00000200) {
                std::cout << "    Compressed: YES\n";
                
                // Try to read first few bytes of compressed data
                std::vector<uint8_t> raw_data;
                if (loader.extractRawCompressedData(filename, raw_data)) {
                    std::cout << "    First 16 bytes of compressed data:\n    ";
                    for (size_t i = 0; i < std::min(size_t(16), raw_data.size()); i++) {
                        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                                  << (int)raw_data[i] << " ";
                    }
                    std::cout << std::dec << "\n";
                    
                    // Check compression mask
                    if (raw_data.size() > 0) {
                        uint8_t compression_mask = raw_data[0];
                        std::cout << "    Compression mask: 0x" << std::hex 
                                  << (int)compression_mask << std::dec << "\n";
                        
                        if (compression_mask & 0x02) std::cout << "      - Zlib\n";
                        if (compression_mask & 0x08) std::cout << "      - PKWARE\n";
                        if (compression_mask & 0x10) std::cout << "      - BZip2\n";
                        if (compression_mask & 0x20) std::cout << "      - SPARSE\n";
                        if (compression_mask & 0x40) std::cout << "      - ADPCM (mono)\n";
                        if (compression_mask & 0x80) std::cout << "      - ADPCM (stereo)\n";
                    }
                }
            } else {
                std::cout << "    Compressed: NO\n";
            }
            
            if (block.flags & 0x00010000) {
                std::cout << "    Encrypted: YES\n";
            } else {
                std::cout << "    Encrypted: NO\n";
            }
            
            if (block.flags & 0x01000000) {
                std::cout << "    Single unit: YES\n";
            } else {
                std::cout << "    Single unit: NO (sector-based)\n";
            }
        }
        
        std::cout << "\n";
    }
    
    return 0;
}