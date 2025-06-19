#include <iostream>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <mpq_file>\n";
        return 1;
    }
    
    MPQLoader loader;
    std::cout << "Opening MPQ: " << argv[1] << "\n";
    
    if (!loader.open(argv[1])) {
        std::cout << "Failed to open MPQ: " << loader.getLastError() << "\n";
        return 1;
    }
    
    // Try to extract a known BZip2 file
    const char* testFile = "data\\global\\excel\\itemstatcost.txt";
    std::cout << "\nTrying to extract: " << testFile << "\n";
    
    std::vector<uint8_t> output;
    if (loader.extractFile(testFile, output)) {
        std::cout << "Success! Extracted " << output.size() << " bytes\n";
        
        // Show first few characters
        std::cout << "First 100 chars: ";
        for (size_t i = 0; i < 100 && i < output.size(); i++) {
            if (output[i] >= 32 && output[i] < 127) {
                std::cout << (char)output[i];
            } else {
                std::cout << ".";
            }
        }
        std::cout << "\n";
    } else {
        std::cout << "Failed: " << loader.getLastError() << "\n";
    }
    
    return 0;
}