#include <iostream>
#include <vector>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"
#include "../engine/include/sprites/dc6_parser.h"

using namespace d2;
using namespace d2portable::sprites;

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
    
    std::cout << "=== Testing DC6 Extraction ===\n\n";
    
    // List of DC6 files to test
    std::vector<std::string> dc6_files = {
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\global\\ui\\cursor\\hand.dc6",
        "data\\local\\font\\latin\\font8.dc6",
        "data\\local\\font\\latin\\font16.dc6",
        "data\\global\\ui\\panel\\invchar6.dc6"
    };
    
    // First, check if these files exist in the MPQ
    std::cout << "Checking for DC6 files in MPQ:\n";
    for (const auto& file : dc6_files) {
        if (loader.hasFile(file)) {
            std::cout << "  ✓ Found: " << file << std::endl;
            
            // Get file info
            auto files = loader.listFiles();
            for (const auto& info : files) {
                if (info.filename == file) {
                    std::cout << "    Size: " << info.uncompressed_size << " bytes (compressed: " 
                              << info.compressed_size << ")\n";
                    break;
                }
            }
        } else {
            std::cout << "  ✗ Not found: " << file << std::endl;
        }
    }
    
    // Try to extract the first found DC6
    std::cout << "\nTrying to extract first DC6 file...\n";
    for (const auto& file : dc6_files) {
        if (loader.hasFile(file)) {
            std::cout << "Extracting: " << file << std::endl;
            
            std::vector<uint8_t> data;
            if (loader.extractFile(file, data)) {
                std::cout << "✅ Extraction succeeded! Got " << data.size() << " bytes\n";
                
                // Show DC6 header
                if (data.size() >= 24) {
                    std::cout << "DC6 Header:\n";
                    uint32_t version = *reinterpret_cast<const uint32_t*>(&data[0]);
                    uint32_t flags = *reinterpret_cast<const uint32_t*>(&data[4]);
                    uint32_t encoding = *reinterpret_cast<const uint32_t*>(&data[8]);
                    uint32_t termination = *reinterpret_cast<const uint32_t*>(&data[12]);
                    uint32_t directions = *reinterpret_cast<const uint32_t*>(&data[16]);
                    uint32_t frames = *reinterpret_cast<const uint32_t*>(&data[20]);
                    
                    std::cout << "  Version: " << version << " (should be 6)\n";
                    std::cout << "  Flags: 0x" << std::hex << flags << std::dec << "\n";
                    std::cout << "  Encoding: " << encoding << "\n";
                    std::cout << "  Termination: " << termination << "\n";
                    std::cout << "  Directions: " << directions << "\n";
                    std::cout << "  Frames per direction: " << frames << "\n";
                    
                    // Try to parse it
                    DC6Parser parser;
                    auto sprite = parser.parseData(data);
                    if (sprite) {
                        std::cout << "\n✅ DC6 parsing succeeded!\n";
                        std::cout << "  Parsed directions: " << sprite->getDirectionCount() << "\n";
                        std::cout << "  Parsed frames: " << sprite->getFramesPerDirection() << "\n";
                    } else {
                        std::cout << "\n❌ DC6 parsing failed!\n";
                    }
                }
                
                break; // Only test first file
            } else {
                std::cout << "❌ Extraction failed: " << loader.getLastError() << std::endl;
            }
        }
    }
    
    // Also look for any DC6 files in the MPQ
    std::cout << "\nSearching for all DC6 files in MPQ:\n";
    auto all_files = loader.listFiles();
    int dc6_count = 0;
    for (const auto& info : all_files) {
        if (info.filename.find(".dc6") != std::string::npos) {
            dc6_count++;
            if (dc6_count <= 10) { // Show first 10
                std::cout << "  " << info.filename << " (" << info.uncompressed_size << " bytes)\n";
            }
        }
    }
    std::cout << "Total DC6 files found: " << dc6_count << std::endl;
    
    return 0;
}