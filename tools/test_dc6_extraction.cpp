#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include "../engine/include/utils/mpq_loader.h"
#include "../engine/include/sprites/dc6_parser.h"

using namespace d2portable::utils;
using namespace d2portable::sprites;

void saveFrameAsPNG(const std::vector<uint8_t>& rgba_data, int width, int height, const std::string& filename) {
    // Simple PPM format for now (can be converted to PNG later)
    std::ofstream file(filename + ".ppm", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to create output file: " << filename << ".ppm\n";
        return;
    }
    
    // PPM header
    file << "P6\n" << width << " " << height << "\n255\n";
    
    // Convert RGBA to RGB for PPM
    for (size_t i = 0; i < rgba_data.size(); i += 4) {
        file.write((char*)&rgba_data[i], 3);  // Write RGB, skip A
    }
    
    file.close();
    std::cout << "Saved frame to: " << filename << ".ppm\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file> [output_dir]\n";
        std::cerr << "Example: " << argv[0] << " d2data.mpq ./output\n";
        return 1;
    }
    
    std::string output_dir = argc > 2 ? argv[2] : "./dc6_output";
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << "\n";
        return 1;
    }
    
    std::cout << "MPQ opened successfully!\n\n";
    
    // List of DC6 files to try extracting
    std::vector<std::string> dc6_files = {
        // UI cursors and fonts
        "data\\global\\ui\\cursor\\cursor.dc6",
        "data\\global\\ui\\fonts\\font8.dc6",
        "data\\global\\ui\\fonts\\font16.dc6",
        
        // Character animations
        "data\\global\\chars\\am\\cof\\amblcof.cof",
        "data\\global\\chars\\am\\tr\\amtrlit.dc6",
        "data\\global\\chars\\ba\\tr\\batrlit.dc6",
        "data\\global\\chars\\ne\\tr\\netrlit.dc6",
        "data\\global\\chars\\pa\\tr\\patrlit.dc6",
        "data\\global\\chars\\so\\tr\\sotrlit.dc6",
        
        // Items
        "data\\global\\items\\misc\\gold\\gold01.dc6",
        "data\\global\\items\\misc\\potion\\hps1.dc6",
        "data\\global\\items\\misc\\potion\\mps1.dc6",
        
        // Monsters
        "data\\global\\monsters\\sk\\tr\\sktrlit.dc6",
        "data\\global\\monsters\\fk\\tr\\fktrlit.dc6",
        
        // UI elements
        "data\\global\\ui\\panel\\invchar6.dc6",
        "data\\global\\ui\\panel\\800ctrlpnl7.dc6",
        
        // Try without data prefix
        "global\\ui\\cursor\\cursor.dc6",
        "global\\ui\\fonts\\font8.dc6"
    };
    
    int extracted_count = 0;
    DC6Parser parser;
    
    for (const auto& filename : dc6_files) {
        std::cout << "\nTrying: " << filename << "\n";
        
        if (!loader.hasFile(filename)) {
            std::cout << "  Not found in MPQ\n";
            continue;
        }
        
        std::vector<uint8_t> data;
        if (!loader.extractFile(filename, data)) {
            std::cout << "  Extraction failed: " << loader.getLastError() << "\n";
            continue;
        }
        
        std::cout << "  Extracted " << data.size() << " bytes\n";
        
        // Try to parse as DC6
        if (!parser.parse(data)) {
            std::cout << "  Failed to parse as DC6: " << parser.getLastError() << "\n";
            continue;
        }
        
        auto info = parser.getInfo();
        std::cout << "  DC6 Info:\n";
        std::cout << "    Version: " << info.version << "\n";
        std::cout << "    Directions: " << info.directions << "\n";
        std::cout << "    Frames per direction: " << info.frames_per_direction << "\n";
        std::cout << "    Total frames: " << info.getTotalFrames() << "\n";
        
        // Extract first frame
        if (info.getTotalFrames() > 0) {
            auto frame_info = parser.getFrameInfo(0);
            std::cout << "    Frame 0: " << frame_info.width << "x" << frame_info.height << "\n";
            
            // Use default palette if no palette loaded
            std::vector<uint8_t> palette;
            std::vector<uint8_t> rgba_data;
            
            if (parser.extractFrame(0, palette, rgba_data)) {
                std::cout << "    Frame extracted successfully (" << rgba_data.size() << " bytes RGBA)\n";
                
                // Save the frame
                std::string base_name = filename.substr(filename.find_last_of("\\") + 1);
                base_name = base_name.substr(0, base_name.find_last_of("."));
                saveFrameAsPNG(rgba_data, frame_info.width, frame_info.height, 
                              output_dir + "/" + base_name + "_frame0");
                
                extracted_count++;
            } else {
                std::cout << "    Failed to extract frame: " << parser.getLastError() << "\n";
            }
        }
    }
    
    std::cout << "\n\nExtracted " << extracted_count << " DC6 sprites successfully!\n";
    std::cout << "Output files saved to: " << output_dir << "\n";
    
    // Also try to find palettes
    std::cout << "\n\nLooking for palette files...\n";
    std::vector<std::string> palette_files = {
        "data\\global\\palette\\act1\\pal.dat",
        "data\\global\\palette\\units\\pal.dat",
        "global\\palette\\act1\\pal.dat",
        "global\\palette\\units\\pal.dat"
    };
    
    for (const auto& pal_file : palette_files) {
        if (loader.hasFile(pal_file)) {
            std::cout << "✓ Found palette: " << pal_file << "\n";
            std::vector<uint8_t> pal_data;
            if (loader.extractFile(pal_file, pal_data)) {
                std::cout << "  Size: " << pal_data.size() << " bytes\n";
                if (pal_data.size() >= 768) {
                    std::cout << "  Valid palette size (256 colors x 3 channels)\n";
                }
            }
        }
    }
    
    return 0;
}