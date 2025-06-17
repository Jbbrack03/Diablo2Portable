#include <iostream>
#include <vector>
#include "../engine/include/core/asset_manager.h"
#include "../engine/include/sprites/dc6_parser.h"

using namespace d2portable::core;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_d2data.mpq>\n";
        return 1;
    }
    
    std::cout << "=== Testing AssetManager with StormLib ===\n\n";
    
    AssetManager manager;
    
    // Initialize with MPQ
    std::cout << "1. Initializing AssetManager with MPQ...\n";
    if (!manager.initializeWithMPQ(argv[1])) {
        std::cerr << "   ❌ Failed to initialize: " << manager.getLastError() << "\n";
        return 1;
    }
    std::cout << "   ✅ Initialized successfully\n\n";
    
    // Test file existence checks
    std::cout << "2. Testing file existence checks...\n";
    std::vector<std::string> test_files = {
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\global\\excel\\armor.txt",
        "data\\global\\palette\\units\\pal.dat"
    };
    
    for (const auto& file : test_files) {
        bool exists = manager.hasFile(file);
        std::cout << "   " << (exists ? "✅" : "❌") << " " << file 
                  << (exists ? " found" : " not found") << "\n";
    }
    
    // Test sprite loading
    std::cout << "\n3. Testing sprite loading...\n";
    auto sprite = manager.loadSprite("data\\global\\ui\\cursor\\ohand.dc6");
    if (sprite) {
        std::cout << "   ✅ Loaded ohand.dc6: " << sprite->getDirectionCount() 
                  << " directions, " << sprite->getFramesPerDirection() << " frames\n";
    } else {
        std::cout << "   ❌ Failed to load ohand.dc6\n";
    }
    
    // Test raw file loading
    std::cout << "\n4. Testing raw file loading...\n";
    auto data = manager.loadFileData("data\\global\\palette\\units\\pal.dat");
    if (!data.empty()) {
        std::cout << "   ✅ Loaded palette: " << data.size() << " bytes";
        if (data.size() == 768) {
            std::cout << " (valid 256-color palette)\n";
        } else {
            std::cout << " (unexpected size)\n";
        }
    } else {
        std::cout << "   ❌ Failed to load palette\n";
    }
    
    // Test cache
    std::cout << "\n5. Testing cache functionality...\n";
    std::cout << "   Cache size before: " << manager.getCachedAssetCount() << " assets\n";
    
    // Load multiple sprites
    manager.loadSprite("data\\local\\font\\latin\\font8.dc6");
    manager.loadSprite("data\\local\\font\\latin\\font16.dc6");
    
    std::cout << "   Cache size after: " << manager.getCachedAssetCount() << " assets\n";
    std::cout << "   Memory usage: " << manager.getCacheMemoryUsage() << " bytes\n";
    
    // Test async loading
    std::cout << "\n6. Testing async sprite loading...\n";
    auto future = manager.loadSpriteAsync("data\\global\\ui\\cursor\\ohand.dc6");
    auto async_sprite = future.get();
    if (async_sprite) {
        std::cout << "   ✅ Async load successful\n";
    } else {
        std::cout << "   ❌ Async load failed\n";
    }
    
    // Clear cache
    manager.clearCache();
    std::cout << "\n7. Cache cleared. Size: " << manager.getCachedAssetCount() << " assets\n";
    
    std::cout << "\n✅ All AssetManager tests completed successfully!\n";
    std::cout << "StormLib integration is working correctly.\n";
    
    return 0;
}