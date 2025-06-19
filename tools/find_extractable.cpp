#include <iostream>
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
    std::cout << "Scanning " << files.size() << " files for extractable content...\n\n";
    
    int extractable_count = 0;
    int shown = 0;
    
    for (const auto& file : files) {
        // Skip if encrypted
        if (file.flags & 0x00010000) continue;
        
        // Skip if compressed (for now, try only uncompressed)
        if (file.flags & 0x00000200) continue;
        
        // Try to extract
        std::vector<uint8_t> data;
        if (loader.extractFile(file.filename, data)) {
            extractable_count++;
            
            if (shown < 10) {
                std::cout << "✓ " << file.filename << " (" << data.size() << " bytes)\n";
                
                // Show first few bytes if it looks like text
                bool looks_like_text = true;
                for (int i = 0; i < 20 && i < data.size(); i++) {
                    if (data[i] < 32 && data[i] != '\n' && data[i] != '\r' && data[i] != '\t') {
                        looks_like_text = false;
                        break;
                    }
                }
                
                if (looks_like_text && data.size() > 0) {
                    std::cout << "  Content: \"";
                    for (int i = 0; i < 50 && i < data.size(); i++) {
                        if (data[i] >= 32 || data[i] == '\n' || data[i] == '\r' || data[i] == '\t') {
                            std::cout << (char)data[i];
                        }
                    }
                    std::cout << "...\"\n";
                }
                
                shown++;
            }
        }
    }
    
    std::cout << "\nTotal extractable files: " << extractable_count << " / " << files.size() << "\n";
    
    return 0;
}