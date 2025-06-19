#include <iostream>
#include <fstream>
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
    
    std::cout << "Attempting to extract (listfile)..." << std::endl;
    
    std::vector<uint8_t> listfile_data;
    bool success = loader.extractFile("(listfile)", listfile_data);
    
    if (!success) {
        std::cout << "Failed to extract (listfile)" << std::endl;
        std::cout << "Error: " << loader.getLastError() << std::endl;
        
        // Try uppercase
        success = loader.extractFile("(LISTFILE)", listfile_data);
        if (!success) {
            std::cout << "Failed to extract (LISTFILE) too" << std::endl;
            return 1;
        }
    }
    
    std::cout << "Successfully extracted listfile!" << std::endl;
    std::cout << "Size: " << listfile_data.size() << " bytes" << std::endl;
    
    // Convert to string and show first few lines
    std::string content(listfile_data.begin(), listfile_data.end());
    std::cout << "\nFirst 10 lines:" << std::endl;
    
    size_t pos = 0;
    int lines = 0;
    while (pos < content.size() && lines < 10) {
        size_t newline = content.find('\n', pos);
        if (newline == std::string::npos) {
            newline = content.size();
        }
        
        std::string line = content.substr(pos, newline - pos);
        // Remove carriage return if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        std::cout << "  " << line << std::endl;
        lines++;
        pos = newline + 1;
    }
    
    // Count total lines
    int total_lines = 0;
    pos = 0;
    while (pos < content.size()) {
        size_t newline = content.find('\n', pos);
        if (newline == std::string::npos) {
            newline = content.size();
        }
        total_lines++;
        pos = newline + 1;
    }
    
    std::cout << "\nTotal lines in listfile: " << total_lines << std::endl;
    
    // Check for specific files
    std::cout << "\nSearching for specific files:" << std::endl;
    const char* search_files[] = {
        "data\\global\\excel\\armor.txt",
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\global\\palette\\act1\\pal.dat"
    };
    
    for (const auto& search : search_files) {
        if (content.find(search) != std::string::npos) {
            std::cout << "  FOUND: " << search << std::endl;
        } else {
            std::cout << "  NOT FOUND: " << search << std::endl;
        }
    }
    
    return 0;
}