#include <iostream>
#include <vector>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2;

// Function to analyze PKWARE distance issues
void analyzePKWAREDistance() {
    std::cout << "=== PKWARE Distance Analysis ===\n\n";
    
    // The issue from CLAUDE.md:
    // Position 23: distance=55 length=3
    // This means at output position 23, we want to copy 3 bytes from position (23-55) = -32
    
    std::cout << "Problem case from CLAUDE.md:\n";
    std::cout << "  Output position: 23\n";
    std::cout << "  Distance: 55\n";
    std::cout << "  Length: 3\n";
    std::cout << "  Source position: 23 - 55 = " << (23 - 55) << " (NEGATIVE!)\n\n";
    
    std::cout << "This suggests one of the following:\n";
    std::cout << "1. The output buffer is pre-initialized with some pattern\n";
    std::cout << "2. The distance calculation is incorrect\n";
    std::cout << "3. There's an offset that needs to be applied\n";
    std::cout << "4. The dictionary window wraps around\n\n";
    
    // Let's think about what position -32 could mean
    // In a 4096-byte window, -32 could wrap to 4096 - 32 = 4064
    std::cout << "If using a 4096-byte circular buffer:\n";
    std::cout << "  Wrapped position: 4096 - 32 = 4064\n\n";
    
    // Or it could be that the first part of the output is pre-filled
    std::cout << "Another possibility: Dictionary pre-fill\n";
    std::cout << "Some PKWARE implementations pre-fill the dictionary with:\n";
    std::cout << "- Spaces (0x20)\n";
    std::cout << "- Zeros (0x00)\n";
    std::cout << "- A repeating pattern\n";
}

int main(int argc, char* argv[]) {
    analyzePKWAREDistance();
    
    if (argc > 1) {
        std::cout << "\n=== Testing with real MPQ ===\n";
        
        MPQLoader loader;
        if (!loader.open(argv[1])) {
            std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
            return 1;
        }
        
        // Try to find a small PKWARE-compressed file
        auto files = loader.listFiles();
        for (const auto& info : files) {
            // Look for small files that might use PKWARE
            if (info.compressed_size > 0 && 
                info.compressed_size < info.uncompressed_size &&
                info.compressed_size < 1000) {
                
                std::cout << "\nTrying: " << info.filename << "\n";
                std::cout << "  Compressed: " << info.compressed_size << " bytes\n";
                std::cout << "  Uncompressed: " << info.uncompressed_size << " bytes\n";
                
                std::vector<uint8_t> data;
                bool success = loader.extractFile(info.filename, data);
                
                if (!success) {
                    std::cout << "  ❌ Failed: " << loader.getLastError() << "\n";
                } else {
                    std::cout << "  ✅ Success! Got " << data.size() << " bytes\n";
                }
                
                // Only test a few files
                static int count = 0;
                if (++count >= 5) break;
            }
        }
    }
    
    return 0;
}