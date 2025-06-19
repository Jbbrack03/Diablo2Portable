#include <iostream>
#include <vector>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <path_to_mpq> <dc6_file>\n";
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::string filename = argv[2];
    std::vector<uint8_t> data;
    
    if (!loader.extractFile(filename, data)) {
        std::cerr << "Failed to extract: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "Extracted " << data.size() << " bytes\n\n";
    
    // Dump first 256 bytes
    std::cout << "First 256 bytes:\n";
    for (size_t i = 0; i < std::min(size_t(256), data.size()); i++) {
        if (i % 16 == 0) {
            std::cout << std::hex << std::setw(4) << std::setfill('0') << i << ": ";
        }
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
        if ((i + 1) % 16 == 0) {
            // Print ASCII representation
            std::cout << " |";
            for (int j = i - 15; j <= i; j++) {
                char c = data[j];
                std::cout << (isprint(c) ? c : '.');
            }
            std::cout << "|\n";
        }
    }
    std::cout << std::dec << "\n";
    
    // Check DC6 header at correct position
    if (data.size() >= 24) {
        std::cout << "\nDC6 Header (first 24 bytes as uint32_t):\n";
        for (size_t i = 0; i < 6; i++) {
            uint32_t value = *reinterpret_cast<const uint32_t*>(&data[i * 4]);
            std::cout << "  Offset " << i*4 << ": " << value;
            if (i == 0) std::cout << " (version, should be 6)";
            else if (i == 1) std::cout << " (flags)";
            else if (i == 2) std::cout << " (encoding)";
            else if (i == 3) std::cout << " (termination)";
            else if (i == 4) std::cout << " (directions)";
            else if (i == 5) std::cout << " (frames per direction)";
            std::cout << "\n";
        }
    }
    
    return 0;
}