#include <iostream>
#include <vector>
#include <iomanip>
#include "../engine/include/utils/stormlib_mpq_loader.h"
#include "../engine/include/sprites/dc6_parser.h"

using namespace d2portable::utils;
using namespace d2portable::sprites;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_d2data.mpq>\n";
        return 1;
    }
    
    StormLibMPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "=== Testing StormLib DC6 Extraction ===\n\n";
    
    // Test DC6 files
    std::vector<std::string> dc6_files = {
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\local\\font\\latin\\font8.dc6",
        "data\\local\\font\\latin\\font16.dc6"
    };
    
    int success_count = 0;
    
    for (const auto& filename : dc6_files) {
        std::cout << "Testing: " << filename << "\n";
        
        if (!loader.hasFile(filename)) {
            std::cout << "  ❌ File not found\n\n";
            continue;
        }
        
        // Get file info
        auto info = loader.getFileInfo(filename);
        if (info) {
            std::cout << "  Size: " << info->uncompressed_size << " bytes";
            if (info->flags & 0x00000200) {
                std::cout << " (compressed from " << info->compressed_size << ")";
            }
            std::cout << "\n";
        }
        
        // Extract file
        std::vector<uint8_t> data;
        if (!loader.extractFile(filename, data)) {
            std::cout << "  ❌ Extraction failed: " << loader.getLastError() << "\n\n";
            continue;
        }
        
        std::cout << "  ✅ Extraction successful! Got " << data.size() << " bytes\n";
        
        // Show first 64 bytes
        std::cout << "  First 64 bytes:\n  ";
        for (size_t i = 0; i < std::min(size_t(64), data.size()); i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << (int)data[i] << " ";
            if ((i + 1) % 16 == 0 && i < 63) std::cout << "\n  ";
        }
        std::cout << std::dec << "\n";
        
        // Check DC6 header
        if (data.size() >= 24) {
            uint32_t version = *reinterpret_cast<const uint32_t*>(&data[0]);
            uint32_t unknown1 = *reinterpret_cast<const uint32_t*>(&data[4]);
            uint32_t unknown2 = *reinterpret_cast<const uint32_t*>(&data[8]);
            uint32_t termination = *reinterpret_cast<const uint32_t*>(&data[12]);
            uint32_t directions = *reinterpret_cast<const uint32_t*>(&data[16]);
            uint32_t frames = *reinterpret_cast<const uint32_t*>(&data[20]);
            
            std::cout << "\n  DC6 Header:\n";
            std::cout << "    Version: " << version << " (should be 6)\n";
            std::cout << "    Unknown1: " << unknown1 << " (should be 1)\n";
            std::cout << "    Unknown2: " << unknown2 << " (should be 0)\n";
            std::cout << "    Termination: 0x" << std::hex << termination << std::dec;
            if (termination == 0xEEEEEEEE) std::cout << " (valid EE)";
            else if (termination == 0xCDCDCDCD) std::cout << " (valid CD)";
            else std::cout << " (INVALID)";
            std::cout << "\n";
            std::cout << "    Directions: " << directions << "\n";
            std::cout << "    Frames per dir: " << frames << "\n";
            
            // Try to parse with DC6Parser
            DC6Parser parser;
            auto sprite = parser.parseData(data);
            if (sprite) {
                std::cout << "\n  ✅ DC6 parsing successful!\n";
                std::cout << "    Parsed directions: " << sprite->getDirectionCount() << "\n";
                std::cout << "    Parsed frames: " << sprite->getFramesPerDirection() << "\n";
                success_count++;
            } else {
                std::cout << "\n  ❌ DC6 parsing failed!\n";
            }
        }
        
        std::cout << "\n";
    }
    
    // List all files to verify we can see everything
    std::cout << "=== Listing Files ===\n";
    auto files = loader.listFiles();
    std::cout << "Total files in MPQ: " << files.size() << "\n";
    
    // Count DC6 files
    int dc6_count = 0;
    for (const auto& file : files) {
        if (file.filename.find(".dc6") != std::string::npos) {
            dc6_count++;
        }
    }
    std::cout << "DC6 files found: " << dc6_count << "\n";
    
    // Summary
    std::cout << "\n=== Summary ===\n";
    std::cout << "Successfully parsed " << success_count << " out of " 
              << dc6_files.size() << " DC6 files\n";
    
    return success_count == dc6_files.size() ? 0 : 1;
}