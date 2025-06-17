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
    
    std::cout << "=== Checking DC6 File Compression ===\n\n";
    
    // Check specific DC6 files
    std::vector<std::string> dc6_files = {
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\local\\font\\latin\\font8.dc6",
        "data\\local\\font\\latin\\font16.dc6"
    };
    
    auto all_files = loader.listFiles();
    
    for (const auto& filename : dc6_files) {
        // Find this file in the list
        for (const auto& info : all_files) {
            if (info.filename == filename) {
                std::cout << "File: " << filename << "\n";
                std::cout << "  Compressed size: " << info.compressed_size << " bytes\n";
                std::cout << "  Uncompressed size: " << info.uncompressed_size << " bytes\n";
                std::cout << "  Flags: 0x" << std::hex << info.flags << std::dec << "\n";
                
                // Check compression flags
                if (info.flags & 0x00000200) {
                    std::cout << "  Compressed: YES\n";
                } else {
                    std::cout << "  Compressed: NO\n";
                }
                
                if (info.flags & 0x00010000) {
                    std::cout << "  Encrypted: YES\n";
                } else {
                    std::cout << "  Encrypted: NO\n";
                }
                
                // Try to get raw compressed data to check compression type
                // This would require modifying MPQLoader to expose raw data
                std::cout << "\n";
                break;
            }
        }
    }
    
    // Count DC6 files by compression
    std::cout << "\nDC6 Files Analysis:\n";
    int total_dc6 = 0;
    int compressed_dc6 = 0;
    int encrypted_dc6 = 0;
    
    for (const auto& info : all_files) {
        if (info.filename.find(".dc6") != std::string::npos) {
            total_dc6++;
            if (info.flags & 0x00000200) compressed_dc6++;
            if (info.flags & 0x00010000) encrypted_dc6++;
        }
    }
    
    std::cout << "Total DC6 files: " << total_dc6 << "\n";
    std::cout << "Compressed DC6 files: " << compressed_dc6 << "\n";
    std::cout << "Encrypted DC6 files: " << encrypted_dc6 << "\n";
    
    return 0;
}