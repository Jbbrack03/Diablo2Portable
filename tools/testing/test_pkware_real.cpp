#include <iostream>
#include <vector>
#include <fstream>
#include "../engine/include/utils/mpq_loader.h"
#include "../engine/include/utils/pkware_explode.h"

using namespace d2;

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
    
    std::cout << "=== Testing PKWARE DCL with Real Files ===\n\n";
    
    // Test a specific file that should use PKWARE compression
    std::string test_file = "data\\global\\palette\\act1\\pal.dat";
    
    if (!loader.hasFile(test_file)) {
        std::cout << "Test file not found: " << test_file << std::endl;
        return 1;
    }
    
    std::cout << "Testing file: " << test_file << std::endl;
    
    // Get the file info to understand compression
    auto files = loader.listFiles();
    for (const auto& fileInfo : files) {
        if (fileInfo.filename == test_file) {
            std::cout << "File info:\n";
            std::cout << "  Compressed size: " << fileInfo.compressed_size << " bytes\n";
            std::cout << "  Uncompressed size: " << fileInfo.uncompressed_size << " bytes\n";
            std::cout << "  Flags: 0x" << std::hex << fileInfo.flags << std::dec << std::endl;
            break;
        }
    }
    
    // Try to extract the file and see what error we get
    std::vector<uint8_t> data;
    bool success = loader.extractFile(test_file, data);
    
    if (success) {
        std::cout << "✅ Extraction succeeded! Got " << data.size() << " bytes\n";
        
        // Show first few bytes
        std::cout << "First 16 bytes: ";
        for (size_t i = 0; i < std::min((size_t)16, data.size()); i++) {
            std::cout << std::hex << (int)data[i] << " ";
        }
        std::cout << std::dec << std::endl;
        
    } else {
        std::cout << "❌ Extraction failed: " << loader.getLastError() << std::endl;
        
        // Let's try to manually debug the compression
        // We need to access the raw compressed data somehow
        std::cout << "\nDebugging PKWARE compression manually...\n";
        
        // This would require modifying MPQLoader to expose raw data
        // For now, let's just document what we learned
    }
    
    // Test a few more palette files
    std::vector<std::string> more_files = {
        "data\\global\\palette\\act2\\pal.dat",
        "data\\global\\palette\\act3\\pal.dat",
        "data\\global\\palette\\loading\\pal.dat"
    };
    
    std::cout << "\nTesting additional palette files:\n";
    for (const auto& file : more_files) {
        if (loader.hasFile(file)) {
            std::vector<uint8_t> file_data;
            bool file_success = loader.extractFile(file, file_data);
            std::cout << "  " << file << ": " << (file_success ? "✅ OK" : "❌ FAILED") << std::endl;
            if (!file_success) {
                std::cout << "    Error: " << loader.getLastError() << std::endl;
            }
        }
    }
    
    return 0;
}