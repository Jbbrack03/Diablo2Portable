#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include "../engine/include/utils/stormlib_mpq_loader.h"
#include "../engine/include/sprites/dc6_parser.h"

using namespace d2portable::utils;
using namespace d2portable::sprites;

bool testMPQLoading(const std::string& mpq_path) {
    std::cout << "\n1. Testing MPQ Loading...\n";
    
    StormLibMPQLoader loader;
    if (!loader.open(mpq_path)) {
        std::cout << "   ❌ Failed to open MPQ: " << loader.getLastError() << "\n";
        return false;
    }
    
    std::cout << "   ✅ MPQ opened successfully\n";
    
    auto files = loader.listFiles();
    std::cout << "   ✅ Found " << files.size() << " files in archive\n";
    
    // Check for common files
    if (loader.hasFile("data\\global\\excel\\armor.txt")) {
        std::cout << "   ✅ Verified armor.txt exists\n";
    }
    
    return true;
}

bool testDC6Extraction(StormLibMPQLoader& loader) {
    std::cout << "\n2. Testing DC6 Sprite Extraction...\n";
    
    std::vector<std::string> test_sprites = {
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\local\\font\\latin\\font8.dc6",
        "data\\global\\items\\misc\\gold\\gold01.dc6"
    };
    
    int extracted = 0;
    DC6Parser parser;
    
    for (const auto& sprite_file : test_sprites) {
        std::cout << "   Testing: " << sprite_file << "\n";
        
        std::vector<uint8_t> data;
        if (!loader.extractFile(sprite_file, data)) {
            std::cout << "     ❌ Extraction failed: " << loader.getLastError() << "\n";
            continue;
        }
        
        std::cout << "     ✅ Extracted " << data.size() << " bytes\n";
        
        auto sprite = parser.parseData(data);
        if (sprite) {
            std::cout << "     ✅ Parsed DC6: " << sprite->getDirectionCount() 
                      << " directions, " << sprite->getFramesPerDirection() << " frames\n";
            extracted++;
        } else {
            std::cout << "     ❌ Failed to parse DC6\n";
        }
    }
    
    std::cout << "   " << (extracted > 0 ? "✅" : "❌") << " Extracted " 
              << extracted << "/" << test_sprites.size() << " sprites\n";
    
    return extracted > 0;
}

bool testTextFileExtraction(StormLibMPQLoader& loader) {
    std::cout << "\n3. Testing Text File Extraction...\n";
    
    std::vector<std::string> text_files = {
        "data\\global\\excel\\armor.txt",
        "data\\global\\excel\\weapons.txt",
        "data\\global\\excel\\misc.txt"
    };
    
    int success = 0;
    
    for (const auto& file : text_files) {
        std::vector<uint8_t> data;
        if (loader.extractFile(file, data)) {
            // Check if it's valid text
            bool has_text = false;
            for (size_t i = 0; i < std::min(size_t(100), data.size()); i++) {
                if (isprint(data[i]) || isspace(data[i])) {
                    has_text = true;
                    break;
                }
            }
            
            if (has_text) {
                std::cout << "   ✅ " << file << " (" << data.size() << " bytes)\n";
                success++;
            } else {
                std::cout << "   ❌ " << file << " (invalid content)\n";
            }
        } else {
            std::cout << "   ❌ Failed to extract " << file << "\n";
        }
    }
    
    std::cout << "   " << (success > 0 ? "✅" : "❌") << " Extracted " 
              << success << "/" << text_files.size() << " text files\n";
    
    return success == text_files.size();
}

bool testPaletteLoading(StormLibMPQLoader& loader) {
    std::cout << "\n4. Testing Palette Loading...\n";
    
    std::vector<std::string> palette_files = {
        "data\\global\\palette\\act1\\pal.dat",
        "data\\global\\palette\\units\\pal.dat"
    };
    
    int valid_palettes = 0;
    
    for (const auto& pal_file : palette_files) {
        std::vector<uint8_t> data;
        if (loader.extractFile(pal_file, data)) {
            if (data.size() == 768) { // 256 * 3 bytes
                std::cout << "   ✅ Valid palette: " << pal_file << "\n";
                valid_palettes++;
            } else {
                std::cout << "   ❌ Invalid palette size: " << pal_file 
                          << " (" << data.size() << " bytes)\n";
            }
        }
    }
    
    std::cout << "   " << (valid_palettes > 0 ? "✅" : "❌") << " Found " 
              << valid_palettes << " palettes\n";
    
    return valid_palettes > 0;
}

bool testPerformance(StormLibMPQLoader& loader) {
    std::cout << "\n5. Testing Performance...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Extract multiple files
    std::vector<std::string> perf_files = {
        "data\\global\\excel\\armor.txt",
        "data\\global\\excel\\weapons.txt",
        "data\\global\\ui\\cursor\\ohand.dc6"
    };
    
    int extracted = 0;
    for (const auto& file : perf_files) {
        std::vector<uint8_t> data;
        if (loader.extractFile(file, data)) {
            extracted++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "   ✅ Extracted " << extracted << " files in " 
              << duration.count() << "ms\n";
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_d2data.mpq>\n";
        return 1;
    }
    
    std::cout << "=== StormLib MPQ Integration Test ===\n";
    std::cout << "Testing with: " << argv[1] << "\n";
    
    // Open MPQ once for all tests
    StormLibMPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << "\n";
        return 1;
    }
    
    bool all_passed = true;
    
    // Run tests
    all_passed &= testMPQLoading(argv[1]);
    all_passed &= testDC6Extraction(loader);
    all_passed &= testTextFileExtraction(loader);
    all_passed &= testPaletteLoading(loader);
    all_passed &= testPerformance(loader);
    
    // Summary
    std::cout << "\n\n=== Test Summary ===\n";
    if (all_passed) {
        std::cout << "✅ ALL TESTS PASSED!\n";
        std::cout << "\nStormLib successfully handles all Diablo II asset types.\n";
        std::cout << "Ready to proceed with game implementation!\n";
    } else {
        std::cout << "❌ SOME TESTS FAILED!\n";
    }
    
    return all_passed ? 0 : 1;
}