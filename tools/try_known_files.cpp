#include <iostream>
#include <vector>
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
    
    // Common Diablo II files to try
    std::vector<std::string> known_files = {
        // Palettes
        "data\\global\\palette\\act1\\pal.dat",
        "data\\global\\palette\\act2\\pal.dat", 
        "data\\global\\palette\\act3\\pal.dat",
        "data\\global\\palette\\act4\\pal.dat",
        "data\\global\\palette\\act5\\pal.dat",
        "data\\global\\palette\\endgame\\pal.dat",
        "data\\global\\palette\\fechar\\pal.dat",
        "data\\global\\palette\\loading\\pal.dat",
        "data\\global\\palette\\menu0\\pal.dat",
        "data\\global\\palette\\menu1\\pal.dat",
        "data\\global\\palette\\menu2\\pal.dat",
        "data\\global\\palette\\menu3\\pal.dat",
        "data\\global\\palette\\menu4\\pal.dat",
        "data\\global\\palette\\sky\\pal.dat",
        "data\\global\\palette\\static\\pal.dat",
        "data\\global\\palette\\trademark\\pal.dat",
        "data\\global\\palette\\units\\pal.dat",
        
        // Excel files  
        "data\\global\\excel\\armor.txt",
        "data\\global\\excel\\charstats.txt",
        "data\\global\\excel\\experience.txt",
        "data\\global\\excel\\gamble.txt",
        "data\\global\\excel\\inventory.txt",
        "data\\global\\excel\\itemstatcost.txt",
        "data\\global\\excel\\misc.txt",
        "data\\global\\excel\\missiles.txt",
        "data\\global\\excel\\monstats.txt",
        "data\\global\\excel\\overlay.txt",
        "data\\global\\excel\\runes.txt",
        "data\\global\\excel\\skills.txt",
        "data\\global\\excel\\sounds.txt",
        "data\\global\\excel\\states.txt",
        "data\\global\\excel\\treasureclassex.txt",
        "data\\global\\excel\\uniqueitems.txt",
        "data\\global\\excel\\weapons.txt",
        
        // UI elements
        "data\\global\\ui\\cursor\\cursor.dc6",
        "data\\global\\ui\\fonts\\font8.dc6",
        "data\\global\\ui\\fonts\\font16.dc6",
        "data\\global\\ui\\fonts\\font24.dc6",
        "data\\global\\ui\\fonts\\font30.dc6",
        "data\\global\\ui\\fonts\\font42.dc6",
        "data\\global\\ui\\fonts\\fontformal10.dc6",
        "data\\global\\ui\\fonts\\fontformal11.dc6",
        "data\\global\\ui\\fonts\\fontformal12.dc6",
        
        // Language files
        "data\\local\\lng\\eng\\string.tbl",
        "data\\local\\lng\\eng\\expansionstring.tbl",
        "data\\local\\lng\\eng\\patchstring.tbl",
        
        // Try without data prefix
        "global\\excel\\monstats.txt",
        "global\\palette\\act1\\pal.dat"
    };
    
    std::cout << "Trying " << known_files.size() << " known Diablo II files...\n\n";
    
    int found = 0;
    for (const auto& filename : known_files) {
        if (loader.hasFile(filename)) {
            found++;
            std::cout << "✓ Found: " << filename << "\n";
            
            // Try to extract
            std::vector<uint8_t> data;
            if (loader.extractFile(filename, data)) {
                std::cout << "  Extracted successfully (" << data.size() << " bytes)\n";
                
                // If it's a text file, show a preview
                if (filename.find(".txt") != std::string::npos) {
                    std::cout << "  Preview: \"";
                    for (int i = 0; i < 60 && i < data.size(); i++) {
                        if (data[i] >= 32 || data[i] == '\n' || data[i] == '\r' || data[i] == '\t') {
                            std::cout << (char)data[i];
                        }
                    }
                    std::cout << "...\"\n";
                }
            } else {
                std::cout << "  Extraction failed: " << loader.getLastError() << "\n";
            }
        }
    }
    
    std::cout << "\nFound " << found << " / " << known_files.size() << " files\n";
    
    return 0;
}