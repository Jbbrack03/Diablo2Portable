#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <map>
#include <set>
#include "../engine/include/core/asset_manager.h"
#include "../engine/include/sprites/dc6_parser.h"

using namespace d2portable::core;
using namespace d2portable::sprites;
namespace fs = std::filesystem;

struct CacheConfig {
    std::vector<std::string> mpq_files;
    std::string output_dir;
    bool optimize_sprites = true;
    bool generate_mipmaps = false;
    bool compress_cache = true;
    int max_texture_size = 2048;
};

class AssetCacheBuilder {
public:
    AssetCacheBuilder(const CacheConfig& config) : config_(config) {}
    
    bool build() {
        std::cout << "Asset Cache Builder\n";
        std::cout << std::string(50, '=') << "\n\n";
        
        // Create output directory
        fs::create_directories(config_.output_dir);
        
        // Initialize asset manager with MPQs
        AssetManager asset_manager;
        if (config_.mpq_files.size() == 1) {
            if (!asset_manager.initializeWithMPQ(config_.mpq_files[0])) {
                std::cerr << "Failed to initialize with MPQ: " << config_.mpq_files[0] << "\n";
                return false;
            }
        } else {
            // Assume MPQs are in same directory
            fs::path mpq_dir = fs::path(config_.mpq_files[0]).parent_path();
            if (!asset_manager.initializeWithMPQs(mpq_dir.string())) {
                std::cerr << "Failed to initialize with MPQ directory: " << mpq_dir << "\n";
                return false;
            }
        }
        
        // Build sprite cache
        buildSpriteCache(asset_manager);
        
        // Build data cache
        buildDataCache(asset_manager);
        
        // Generate cache manifest
        generateManifest();
        
        std::cout << "\nCache building complete!\n";
        std::cout << "Output directory: " << config_.output_dir << "\n";
        
        return true;
    }
    
private:
    void buildSpriteCache(AssetManager& asset_manager) {
        std::cout << "Building sprite cache...\n";
        
        // List of important sprites to cache
        std::vector<std::string> priority_sprites = {
            // UI elements (always needed)
            "data\\global\\ui\\cursor\\cursor.dc6",
            "data\\global\\ui\\fonts\\font8.dc6",
            "data\\global\\ui\\fonts\\font16.dc6",
            "data\\global\\ui\\fonts\\font24.dc6",
            "data\\global\\ui\\fonts\\font30.dc6",
            "data\\global\\ui\\fonts\\font42.dc6",
            "data\\global\\ui\\panel\\invchar6.dc6",
            "data\\global\\ui\\panel\\800ctrlpnl7.dc6",
            
            // Character selection screen
            "data\\global\\ui\\FrontEnd\\barbarian\\banu1.dc6",
            "data\\global\\ui\\FrontEnd\\sorceress\\sonu1.dc6",
            "data\\global\\ui\\FrontEnd\\necromancer\\nenu1.dc6",
            "data\\global\\ui\\FrontEnd\\paladin\\panu1.dc6",
            "data\\global\\ui\\FrontEnd\\amazon\\amnu1.dc6",
            "data\\global\\ui\\FrontEnd\\assassin\\asnu1.dc6",
            "data\\global\\ui\\FrontEnd\\druid\\dznu1.dc6",
            
            // Common items
            "data\\global\\items\\misc\\gold\\gold01.dc6",
            "data\\global\\items\\misc\\gold\\gold02.dc6",
            "data\\global\\items\\misc\\gold\\gold03.dc6",
            "data\\global\\items\\misc\\potion\\hps1.dc6",
            "data\\global\\items\\misc\\potion\\mps1.dc6",
            "data\\global\\items\\misc\\potion\\rps1.dc6"
        };
        
        int processed = 0;
        int failed = 0;
        
        for (const auto& sprite_path : priority_sprites) {
            auto sprite = asset_manager.loadSprite(sprite_path);
            if (sprite) {
                if (processSpriteToCache(sprite_path, sprite)) {
                    processed++;
                } else {
                    failed++;
                }
            } else {
                std::cout << "  Skipping (not found): " << sprite_path << "\n";
            }
        }
        
        std::cout << "Sprite cache complete: " << processed << " processed, " 
                  << failed << " failed\n\n";
    }
    
    bool processSpriteToCache(const std::string& sprite_path, 
                             std::shared_ptr<DC6Sprite> sprite) {
        // Convert path to cache filename
        std::string cache_name = sprite_path;
        std::replace(cache_name.begin(), cache_name.end(), '\\', '_');
        std::replace(cache_name.begin(), cache_name.end(), '/', '_');
        
        fs::path cache_path = fs::path(config_.output_dir) / "sprites" / (cache_name + ".cache");
        fs::create_directories(cache_path.parent_path());
        
        // Extract sprite information
        uint32_t directions = sprite->getDirectionCount();
        uint32_t frames_per_dir = sprite->getFramesPerDirection();
        
        std::cout << "  Processing: " << sprite_path 
                  << " (" << directions << " dirs, " 
                  << frames_per_dir << " frames/dir)\n";
        
        // Create cache file
        std::ofstream cache_file(cache_path, std::ios::binary);
        if (!cache_file) {
            return false;
        }
        
        // Write cache header
        const char* magic = "D2SC"; // D2 Sprite Cache
        cache_file.write(magic, 4);
        
        uint32_t version = 1;
        cache_file.write(reinterpret_cast<const char*>(&version), 4);
        cache_file.write(reinterpret_cast<const char*>(&directions), 4);
        cache_file.write(reinterpret_cast<const char*>(&frames_per_dir), 4);
        
        // Process each frame
        for (uint32_t dir = 0; dir < directions; dir++) {
            for (uint32_t frame = 0; frame < frames_per_dir; frame++) {
                auto frame_data = sprite->getFrame(dir, frame);
                auto rgba_data = sprite->getFrameImage(dir, frame);
                
                // Write frame header
                cache_file.write(reinterpret_cast<const char*>(&frame_data.width), 4);
                cache_file.write(reinterpret_cast<const char*>(&frame_data.height), 4);
                cache_file.write(reinterpret_cast<const char*>(&frame_data.offset_x), 4);
                cache_file.write(reinterpret_cast<const char*>(&frame_data.offset_y), 4);
                
                uint32_t data_size = rgba_data.size();
                cache_file.write(reinterpret_cast<const char*>(&data_size), 4);
                
                // Optimize if requested
                if (config_.optimize_sprites) {
                    // Could implement texture compression here
                    // For now, just write raw RGBA
                    cache_file.write(reinterpret_cast<const char*>(rgba_data.data()), 
                                   rgba_data.size());
                } else {
                    cache_file.write(reinterpret_cast<const char*>(rgba_data.data()), 
                                   rgba_data.size());
                }
            }
        }
        
        sprite_cache_entries_[sprite_path] = cache_name + ".cache";
        return true;
    }
    
    void buildDataCache(AssetManager& asset_manager) {
        std::cout << "Building data cache...\n";
        
        // Important data files
        std::vector<std::string> data_files = {
            // Game data tables
            "data\\global\\excel\\armor.txt",
            "data\\global\\excel\\weapons.txt",
            "data\\global\\excel\\misc.txt",
            "data\\global\\excel\\uniqueitems.txt",
            "data\\global\\excel\\setitems.txt",
            "data\\global\\excel\\runes.txt",
            "data\\global\\excel\\gems.txt",
            "data\\global\\excel\\skills.txt",
            "data\\global\\excel\\skilldesc.txt",
            "data\\global\\excel\\monstats.txt",
            "data\\global\\excel\\levels.txt",
            "data\\global\\excel\\experience.txt",
            
            // String tables
            "data\\local\\lng\\eng\\string.tbl",
            "data\\local\\lng\\eng\\patchstring.tbl",
            "data\\local\\lng\\eng\\expansionstring.tbl"
        };
        
        int processed = 0;
        
        for (const auto& data_path : data_files) {
            auto data = asset_manager.loadFileData(data_path);
            if (!data.empty()) {
                // Convert path
                std::string cache_name = data_path;
                std::replace(cache_name.begin(), cache_name.end(), '\\', '/');
                
                fs::path cache_path = fs::path(config_.output_dir) / "data" / cache_name;
                fs::create_directories(cache_path.parent_path());
                
                // Write data
                std::ofstream out(cache_path, std::ios::binary);
                if (out) {
                    out.write(reinterpret_cast<const char*>(data.data()), data.size());
                    data_cache_entries_[data_path] = cache_name;
                    processed++;
                    
                    std::cout << "  Cached: " << data_path 
                              << " (" << data.size() << " bytes)\n";
                }
            }
        }
        
        std::cout << "Data cache complete: " << processed << " files cached\n\n";
    }
    
    void generateManifest() {
        std::cout << "Generating cache manifest...\n";
        
        fs::path manifest_path = fs::path(config_.output_dir) / "cache_manifest.json";
        std::ofstream manifest(manifest_path);
        
        if (!manifest) {
            std::cerr << "Failed to create manifest file\n";
            return;
        }
        
        manifest << "{\n";
        manifest << "  \"version\": 1,\n";
        manifest << "  \"created\": \"" << getCurrentTimestamp() << "\",\n";
        manifest << "  \"config\": {\n";
        manifest << "    \"optimize_sprites\": " 
                 << (config_.optimize_sprites ? "true" : "false") << ",\n";
        manifest << "    \"generate_mipmaps\": " 
                 << (config_.generate_mipmaps ? "true" : "false") << ",\n";
        manifest << "    \"max_texture_size\": " << config_.max_texture_size << "\n";
        manifest << "  },\n";
        
        // Sprite entries
        manifest << "  \"sprites\": {\n";
        bool first = true;
        for (const auto& [orig, cached] : sprite_cache_entries_) {
            if (!first) manifest << ",\n";
            manifest << "    \"" << orig << "\": \"sprites/" << cached << "\"";
            first = false;
        }
        manifest << "\n  },\n";
        
        // Data entries
        manifest << "  \"data\": {\n";
        first = true;
        for (const auto& [orig, cached] : data_cache_entries_) {
            if (!first) manifest << ",\n";
            manifest << "    \"" << orig << "\": \"data/" << cached << "\"";
            first = false;
        }
        manifest << "\n  }\n";
        
        manifest << "}\n";
        
        std::cout << "Manifest created: " << manifest_path << "\n";
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    CacheConfig config_;
    std::map<std::string, std::string> sprite_cache_entries_;
    std::map<std::string, std::string> data_cache_entries_;
};

void printUsage(const char* programName) {
    std::cout << "Asset Cache Builder\n";
    std::cout << "Usage: " << programName << " [options] <mpq_files...> <output_dir>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --no-optimize      Don't optimize sprites\n";
    std::cout << "  --generate-mipmaps Generate mipmaps for textures\n";
    std::cout << "  --max-size <size>  Maximum texture size (default: 2048)\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << programName << " d2data.mpq d2exp.mpq ./cache\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    CacheConfig config;
    
    // Parse arguments
    int arg_index = 1;
    while (arg_index < argc - 1) {
        std::string arg = argv[arg_index];
        
        if (arg == "--no-optimize") {
            config.optimize_sprites = false;
            arg_index++;
        } else if (arg == "--generate-mipmaps") {
            config.generate_mipmaps = true;
            arg_index++;
        } else if (arg == "--max-size" && arg_index + 1 < argc - 1) {
            config.max_texture_size = std::stoi(argv[arg_index + 1]);
            arg_index += 2;
        } else if (arg.substr(0, 2) == "--") {
            std::cerr << "Unknown option: " << arg << "\n";
            return 1;
        } else {
            // MPQ file
            if (fs::exists(arg)) {
                config.mpq_files.push_back(arg);
            } else {
                std::cerr << "MPQ file not found: " << arg << "\n";
                return 1;
            }
            arg_index++;
        }
    }
    
    if (config.mpq_files.empty()) {
        std::cerr << "Error: No MPQ files specified\n";
        return 1;
    }
    
    // Last argument is output directory
    config.output_dir = argv[argc - 1];
    
    // Build cache
    AssetCacheBuilder builder(config);
    if (!builder.build()) {
        std::cerr << "Cache building failed!\n";
        return 1;
    }
    
    return 0;
}