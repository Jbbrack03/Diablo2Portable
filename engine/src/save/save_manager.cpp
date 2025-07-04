#include "game/save_manager.h"
#include "game/character.h"
#include <fstream>

namespace d2::game {

bool SaveManager::saveCharacter(const Character& character, const std::string& filename) {
    // Open file in binary mode
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Write D2S header
    // Signature
    uint32_t signature = 0xAA55AA55;
    file.write(reinterpret_cast<const char*>(&signature), sizeof(signature));
    
    // Version (0x60 = v1.09+)
    uint32_t version = 0x60;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // File size (placeholder)
    uint32_t fileSize = 0;
    file.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
    
    // Checksum (placeholder)
    uint32_t checksum = 0;
    file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
    
    // Character name (16 bytes)
    char name[16] = {0};
    file.write(name, 16);
    
    // Status (1 byte)
    uint8_t status = 0;
    file.write(reinterpret_cast<const char*>(&status), sizeof(status));
    
    // Progression (1 byte)
    uint8_t progression = 0;
    file.write(reinterpret_cast<const char*>(&progression), sizeof(progression));
    
    // Unknown (2 bytes)
    uint16_t unknown = 0;
    file.write(reinterpret_cast<const char*>(&unknown), sizeof(unknown));
    
    // Character class (1 byte)
    uint8_t charClass = static_cast<uint8_t>(character.getCharacterClass());
    file.write(reinterpret_cast<const char*>(&charClass), sizeof(charClass));
    
    // Character level (1 byte)
    uint8_t level = static_cast<uint8_t>(character.getLevel());
    file.write(reinterpret_cast<const char*>(&level), sizeof(level));
    
    file.close();
    
    return true;
}

std::unique_ptr<Character> SaveManager::loadCharacter(const std::string& filename) {
    // Open file in binary mode
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return nullptr;
    }
    
    // Read and verify signature
    uint32_t signature;
    file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
    if (signature != 0xAA55AA55) {
        return nullptr;
    }
    
    // Skip version (4), filesize (4), checksum (4), name (16), 
    // status (1), progression (1), unknown (2) = 32 bytes
    file.seekg(32, std::ios::cur);
    
    // Read character class
    uint8_t charClass;
    file.read(reinterpret_cast<char*>(&charClass), sizeof(charClass));
    
    // Read level
    uint8_t level;
    file.read(reinterpret_cast<char*>(&level), sizeof(level));
    
    // Create character
    auto character = std::make_unique<Character>(static_cast<CharacterClass>(charClass));
    character->setLevel(level);
    
    file.close();
    
    return character;
}

} // namespace d2::game