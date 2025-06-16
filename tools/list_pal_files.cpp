#include <iostream>
#include <string>
#include <algorithm>
#include "utils/mpq_loader.h"

using namespace d2portable::utils;

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
    
    auto files = loader.listFiles();
    std::cout << "Total files: " << files.size() << std::endl;
    
    std::cout << "\nPalette files (.pal and pal.dat):" << std::endl;
    int pal_count = 0;
    for (const auto& file : files) {
        std::string lower = file.filename;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower.find(".pal") != std::string::npos || 
            lower.find("pal.dat") != std::string::npos ||
            lower.find("palette") != std::string::npos) {
            std::cout << "  " << file.filename << " (compressed: " << file.compressed_size 
                     << ", uncompressed: " << file.uncompressed_size << ")" << std::endl;
            pal_count++;
        }
    }
    std::cout << "Found " << pal_count << " palette files" << std::endl;
    
    std::cout << "\nDC6 files:" << std::endl;
    int dc6_count = 0;
    for (const auto& file : files) {
        std::string lower = file.filename;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower.find(".dc6") != std::string::npos) {
            std::cout << "  " << file.filename << std::endl;
            dc6_count++;
            if (dc6_count >= 10) break;
        }
    }
    std::cout << "Total DC6 files: " << dc6_count << std::endl;
    
    std::cout << "\nFirst 20 files:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(20), files.size()); ++i) {
        std::cout << "  " << files[i].filename << std::endl;
    }
    
    return 0;
}