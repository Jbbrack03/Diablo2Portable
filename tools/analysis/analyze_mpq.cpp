#include <iostream>
#include <map>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file>\n";
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open: " << loader.getLastError() << "\n";
        return 1;
    }
    
    auto files = loader.listFiles();
    std::cout << "Analyzing " << files.size() << " files...\n\n";
    
    std::map<uint32_t, int> flag_counts;
    int encrypted = 0;
    int compressed = 0;
    int imploded = 0;
    int plain = 0;
    
    for (const auto& file : files) {
        flag_counts[file.flags]++;
        
        if (file.flags & 0x00010000) encrypted++;
        if (file.flags & 0x00000200) compressed++;
        if (file.flags & 0x00000100) imploded++;
        if ((file.flags & 0xFFFF0000) == 0x80000000) plain++;
    }
    
    std::cout << "File flag distribution:\n";
    for (const auto& [flags, count] : flag_counts) {
        std::cout << "  0x" << std::hex << flags << std::dec 
                  << " : " << count << " files\n";
    }
    
    std::cout << "\nSummary:\n";
    std::cout << "  Encrypted: " << encrypted << "\n";
    std::cout << "  Compressed: " << compressed << "\n";
    std::cout << "  Imploded: " << imploded << "\n";
    std::cout << "  Plain: " << plain << "\n";
    
    // Try to extract a file with only compression
    std::cout << "\nTrying to extract compressed files...\n";
    int tested = 0;
    for (const auto& file : files) {
        if ((file.flags == 0x80000200) && tested < 5) { // Only compressed, not encrypted
            std::vector<uint8_t> data;
            std::cout << "  " << file.filename << " - ";
            if (loader.extractFile(file.filename, data)) {
                std::cout << "SUCCESS (" << data.size() << " bytes)\n";
            } else {
                std::cout << "FAILED: " << loader.getLastError() << "\n";
            }
            tested++;
        }
    }
    
    return 0;
}