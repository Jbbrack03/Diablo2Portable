#include <iostream>
#include <vector>
#include "utils/mpq_loader.h"

using namespace d2;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file>" << std::endl;
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << argv[1] << std::endl;
        return 1;
    }
    
    // Try a simple palette file that we know uses PKWARE
    const char* test_file = "data\\global\\palette\\act1\\pal.dat";
    
    std::cout << "Testing extraction of: " << test_file << std::endl;
    
    std::vector<uint8_t> data;
    bool success = loader.extractFile(test_file, data);
    
    if (success) {
        std::cout << "\nSUCCESS! Extracted " << data.size() << " bytes" << std::endl;
        if (data.size() == 768) {
            std::cout << "Valid palette size (256 colors * 3 bytes)" << std::endl;
        }
    } else {
        std::cout << "\nFAILED: " << loader.getLastError() << std::endl;
    }
    
    return success ? 0 : 1;
}