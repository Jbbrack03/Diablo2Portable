#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2;

void printBytes(const std::vector<uint8_t>& data, size_t count = 32) {
    std::cout << "First " << std::min(count, data.size()) << " bytes:\n";
    for (size_t i = 0; i < std::min(count, data.size()); i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
        if ((i + 1) % 16 == 0) std::cout << "\n";
    }
    std::cout << std::dec << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <path_to_mpq> <file_path>\n";
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::string file_path = argv[2];
    std::cout << "Analyzing: " << file_path << "\n\n";
    
    // Get raw compressed data by modifying the loader
    // For now, let's just extract and see what happens
    std::vector<uint8_t> data;
    bool success = loader.extractFile(file_path, data);
    
    if (success) {
        std::cout << "✅ Extraction succeeded! Got " << data.size() << " bytes\n";
        printBytes(data);
        
        // Check if it's DC6 format
        if (data.size() >= 24) {
            uint32_t version = *(uint32_t*)&data[0];
            uint32_t flags = *(uint32_t*)&data[4];
            std::cout << "\nIf DC6:\n";
            std::cout << "  Version: " << version << " (should be 6)\n";
            std::cout << "  Flags: 0x" << std::hex << flags << std::dec << "\n";
        }
    } else {
        std::cout << "❌ Extraction failed: " << loader.getLastError() << std::endl;
    }
    
    return 0;
}