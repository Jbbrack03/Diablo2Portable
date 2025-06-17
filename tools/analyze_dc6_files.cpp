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
    
    std::cout << "=== Analyzing DC6 Files in MPQ ===\n\n";
    
    // List all files and look for DC6
    auto all_files = loader.listFiles();
    
    int total_dc6 = 0;
    int compressed_dc6 = 0;
    int encrypted_dc6 = 0;
    int single_unit_dc6 = 0;
    
    std::cout << "First 10 DC6 files found:\n";
    for (const auto& info : all_files) {
        if (info.filename.find(".dc6") != std::string::npos) {
            total_dc6++;
            
            if (info.flags & 0x00000200) compressed_dc6++;
            if (info.flags & 0x00010000) encrypted_dc6++;
            if (info.flags & 0x01000000) single_unit_dc6++;
            
            if (total_dc6 <= 10) {
                std::cout << "  " << info.filename << "\n";
                std::cout << "    Flags: 0x" << std::hex << info.flags << std::dec << "\n";
                std::cout << "    Size: " << info.uncompressed_size << " bytes";
                if (info.flags & 0x00000200) {
                    std::cout << " (compressed from " << info.compressed_size << ")";
                }
                std::cout << "\n";
                
                // Try to extract
                std::vector<uint8_t> data;
                if (loader.extractFile(info.filename, data)) {
                    std::cout << "    ✅ Extraction successful!\n";
                    
                    // Check DC6 header
                    if (data.size() >= 4) {
                        uint32_t version = *reinterpret_cast<const uint32_t*>(&data[0]);
                        std::cout << "    DC6 Version: " << version << " (should be 6)\n";
                    }
                } else {
                    std::cout << "    ❌ Extraction failed: " << loader.getLastError() << "\n";
                }
                std::cout << "\n";
            }
        }
    }
    
    std::cout << "\nSummary:\n";
    std::cout << "Total DC6 files: " << total_dc6 << "\n";
    std::cout << "Compressed: " << compressed_dc6 << "\n";
    std::cout << "Encrypted: " << encrypted_dc6 << "\n";
    std::cout << "Single unit: " << single_unit_dc6 << "\n";
    
    // Also check other MPQ files for DC6
    std::cout << "\n=== Checking other MPQs for DC6 files ===\n";
    std::vector<std::string> other_mpqs = {
        "./vendor/mpq/d2char.mpq",
        "./vendor/mpq/d2exp.mpq"
    };
    
    for (const auto& mpq_path : other_mpqs) {
        MPQLoader other_loader;
        if (other_loader.open(mpq_path)) {
            auto other_files = other_loader.listFiles();
            int dc6_count = 0;
            for (const auto& info : other_files) {
                if (info.filename.find(".dc6") != std::string::npos) {
                    dc6_count++;
                }
            }
            std::cout << mpq_path << ": " << dc6_count << " DC6 files\n";
        }
    }
    
    return 0;
}