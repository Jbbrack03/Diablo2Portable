#include "save/save_manager.h"
#include "game/character.h"
#include <filesystem>
#include <fstream>
#include <cstring>
#include <vector>

namespace d2::save {

// D2S file constants
constexpr uint32_t D2S_SIGNATURE = 0xAA55AA55;
constexpr uint32_t D2S_VERSION = 0x60;  // Version 96 (1.09)
constexpr size_t D2S_HEADER_SIZE = 765;

// Calculate D2S checksum using proper rotating left shift algorithm
uint32_t calculateChecksum(std::vector<uint8_t> data) {
    // Zero out the checksum field (offset 12-15)
    std::memset(&data[12], 0, 4);
    
    uint32_t sum = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        // Check if we need to carry the high bit
        uint32_t carry = (sum & 0x80000000) ? 1 : 0;
        
        // Rotate left by 1 (shift left and add carry to lowest bit)
        sum = (sum << 1) | carry;
        
        // Add the current byte
        sum += data[i];
    }
    
    return sum;
}

SaveManager::SaveManager(const std::string& saveDirectory) 
    : m_saveDirectory(saveDirectory) {
    // Create the save directory if it doesn't exist
    std::filesystem::create_directories(saveDirectory);
}

bool SaveManager::saveCharacter(const d2::game::Character& character, const std::string& fileName) {
    // Create full path
    std::filesystem::path savePath = std::filesystem::path(m_saveDirectory) / fileName;
    
    // Open file for binary writing
    std::ofstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Create header buffer
    std::vector<uint8_t> header(D2S_HEADER_SIZE, 0);
    
    // Write signature (offset 0)
    *reinterpret_cast<uint32_t*>(&header[0]) = D2S_SIGNATURE;
    
    // Write version (offset 4)
    *reinterpret_cast<uint32_t*>(&header[4]) = D2S_VERSION;
    
    // Write file size placeholder (offset 8) - will update later
    *reinterpret_cast<uint32_t*>(&header[8]) = 0;
    
    // Checksum placeholder (offset 12) - will calculate later
    *reinterpret_cast<uint32_t*>(&header[12]) = 0;
    
    // Write character name (offset 20)
    const char* defaultName = "TestChar";
    std::strncpy(reinterpret_cast<char*>(&header[20]), defaultName, 16);
    
    // Write character class (offset 40)
    header[40] = static_cast<uint8_t>(character.getCharacterClass());
    
    // Write level (offset 43)
    header[43] = static_cast<uint8_t>(character.getLevel());
    
    // Calculate checksum using proper D2S algorithm
    uint32_t checksum = calculateChecksum(header);
    
    // Write checksum back to header
    *reinterpret_cast<uint32_t*>(&header[12]) = checksum;
    
    // Write header to file
    file.write(reinterpret_cast<const char*>(header.data()), header.size());
    
    // Close file
    file.close();
    
    return true;
}

std::unique_ptr<d2::game::Character> SaveManager::loadCharacter(const std::string& fileName) {
    // Create full path
    std::filesystem::path savePath = std::filesystem::path(m_saveDirectory) / fileName;
    
    // Open file for binary reading
    std::ifstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        return nullptr;
    }
    
    // Read header
    std::vector<uint8_t> header(D2S_HEADER_SIZE);
    file.read(reinterpret_cast<char*>(header.data()), header.size());
    
    if (!file.good()) {
        return nullptr;
    }
    
    // Verify signature
    uint32_t signature = *reinterpret_cast<uint32_t*>(&header[0]);
    if (signature != D2S_SIGNATURE) {
        return nullptr;
    }
    
    // Extract stored checksum before validation
    uint32_t storedChecksum = *reinterpret_cast<uint32_t*>(&header[12]);
    
    // Calculate expected checksum
    uint32_t calculatedChecksum = calculateChecksum(header);
    
    // Validate checksum
    if (storedChecksum != calculatedChecksum) {
        return nullptr;  // Invalid checksum - file is corrupted
    }
    
    // Read character class (offset 40)
    auto charClass = static_cast<d2::game::CharacterClass>(header[40]);
    
    // Read level (offset 43)
    uint8_t level = header[43];
    
    // Create character
    auto character = std::make_unique<d2::game::Character>(charClass);
    character->setLevel(level);
    
    return character;
}

} // namespace d2::save