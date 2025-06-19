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
    
    // Search for DC6 header pattern: 06 00 00 00 01 00 00 00 00 00 00 00
    std::cout << "Searching for DC6 header pattern...\n";
    
    for (size_t i = 0; i + 12 <= data.size(); i++) {
        // Check for version 6
        if (data[i] == 0x06 && data[i+1] == 0x00 && data[i+2] == 0x00 && data[i+3] == 0x00) {
            // Check for unknown1 = 1
            if (data[i+4] == 0x01 && data[i+5] == 0x00 && data[i+6] == 0x00 && data[i+7] == 0x00) {
                // Check for unknown2 = 0
                if (data[i+8] == 0x00 && data[i+9] == 0x00 && data[i+10] == 0x00 && data[i+11] == 0x00) {
                    std::cout << "\nFound potential DC6 header at offset " << i << ":\n";
                    
                    // Dump next 32 bytes
                    for (size_t j = i; j < std::min(i + 32, data.size()); j++) {
                        if ((j - i) % 16 == 0) {
                            std::cout << std::hex << std::setw(4) << std::setfill('0') 
                                      << j << ": ";
                        }
                        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                                  << (int)data[j] << " ";
                        if ((j - i + 1) % 16 == 0) std::cout << "\n";
                    }
                    std::cout << std::dec << "\n";
                    
                    // Check termination bytes
                    if (i + 16 <= data.size()) {
                        uint32_t term = *reinterpret_cast<const uint32_t*>(&data[i + 12]);
                        std::cout << "Termination: 0x" << std::hex << term << std::dec;
                        if (term == 0xEEEEEEEE) std::cout << " (valid EE)";
                        else if (term == 0xCDCDCDCD) std::cout << " (valid CD)";
                        else std::cout << " (invalid)";
                        std::cout << "\n";
                    }
                    
                    if (i + 24 <= data.size()) {
                        uint32_t dirs = *reinterpret_cast<const uint32_t*>(&data[i + 16]);
                        uint32_t frames = *reinterpret_cast<const uint32_t*>(&data[i + 20]);
                        std::cout << "Directions: " << dirs << "\n";
                        std::cout << "Frames per dir: " << frames << "\n";
                    }
                }
            }
        }
    }
    
    // Also search for termination patterns
    std::cout << "\nSearching for termination patterns...\n";
    for (size_t i = 0; i + 4 <= data.size(); i++) {
        if ((data[i] == 0xEE && data[i+1] == 0xEE && data[i+2] == 0xEE && data[i+3] == 0xEE) ||
            (data[i] == 0xCD && data[i+1] == 0xCD && data[i+2] == 0xCD && data[i+3] == 0xCD)) {
            std::cout << "Found termination at offset " << i << ": ";
            for (int j = 0; j < 4; j++) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i+j] << " ";
            }
            std::cout << std::dec << "\n";
        }
    }
    
    return 0;
}