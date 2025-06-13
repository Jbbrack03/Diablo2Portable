#include <iostream>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2portable::utils;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file> <file_to_extract>\n";
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << "\n";
        return 1;
    }
    
    std::string filename = argv[2];
    
    if (loader.hasFile(filename)) {
        std::cout << "File found: " << filename << "\n";
        
        auto info = loader.getFileInfo(filename);
        if (info) {
            std::cout << "  Compressed size: " << info->compressed_size << "\n";
            std::cout << "  Uncompressed size: " << info->uncompressed_size << "\n";
            std::cout << "  Flags: 0x" << std::hex << info->flags << std::dec << "\n";
            std::cout << "  Compressed: " << ((info->flags & 0x200) ? "yes" : "no") << "\n";
        }
        
        // Try to extract and see the compression byte
        std::vector<uint8_t> data;
        if (!loader.extractFile(filename, data)) {
            std::cout << "\nExtraction failed: " << loader.getLastError() << "\n";
            
            // Let's see if we can manually check the compression type
            // This would require internal access, so we can't do it directly
        } else {
            std::cout << "\nExtraction successful!\n";
            std::cout << "First 16 bytes: ";
            for (int i = 0; i < 16 && i < data.size(); i++) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                         << (int)data[i] << " ";
            }
            std::cout << std::dec << "\n";
        }
    } else {
        std::cout << "File not found: " << filename << "\n";
        
        // Try some variations
        std::cout << "\nTrying variations:\n";
        std::vector<std::string> variations = {
            "(listfile)",
            "(ListFile)",
            "data\\global\\excel\\monstats.txt",
            "data\\global\\excel\\MonStats.txt",
            "data/global/excel/monstats.txt"
        };
        
        for (const auto& var : variations) {
            if (loader.hasFile(var)) {
                std::cout << "  Found: " << var << "\n";
            }
        }
    }
    
    return 0;
}