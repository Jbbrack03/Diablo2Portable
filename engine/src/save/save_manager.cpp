#include "save/save_manager.h"
#include "game/character.h"
#include "game/inventory.h"
#include "game/item.h"
#include <filesystem>
#include <fstream>
#include <cstring>
#include <vector>
#include <set>
#include <tuple>

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
    
    // Check if file already exists and create backup if it does
    if (std::filesystem::exists(savePath)) {
        // Create backup directory if it doesn't exist
        std::filesystem::path backupDir = std::filesystem::path(m_saveDirectory) / "backup";
        std::filesystem::create_directories(backupDir);
        
        // Create backup file path
        std::filesystem::path backupPath = backupDir / (fileName + ".bak");
        
        // Copy existing file to backup
        std::filesystem::copy_file(savePath, backupPath, 
                                   std::filesystem::copy_options::overwrite_existing);
    }
    
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
    
    // Write stats at custom offsets (simplified for our implementation)
    // These offsets are in unused parts of the header
    // Strength at offset 100
    *reinterpret_cast<uint16_t*>(&header[100]) = static_cast<uint16_t>(character.getStrength());
    // Dexterity at offset 102
    *reinterpret_cast<uint16_t*>(&header[102]) = static_cast<uint16_t>(character.getDexterity());
    // Vitality at offset 104
    *reinterpret_cast<uint16_t*>(&header[104]) = static_cast<uint16_t>(character.getVitality());
    // Energy at offset 106
    *reinterpret_cast<uint16_t*>(&header[106]) = static_cast<uint16_t>(character.getEnergy());
    
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
    
    // Read stats from custom offsets
    uint16_t strength = *reinterpret_cast<uint16_t*>(&header[100]);
    uint16_t dexterity = *reinterpret_cast<uint16_t*>(&header[102]);
    uint16_t vitality = *reinterpret_cast<uint16_t*>(&header[104]);
    uint16_t energy = *reinterpret_cast<uint16_t*>(&header[106]);
    
    // Apply stats to character (need to subtract base stats first)
    // Get base stats for the class
    d2::game::Character tempChar(charClass);
    int baseStr = tempChar.getStrength();
    int baseDex = tempChar.getDexterity();
    int baseVit = tempChar.getVitality();
    int baseEne = tempChar.getEnergy();
    
    // Add the difference as stat points
    if (strength > baseStr) {
        character->addStatPoint(d2::game::StatType::STRENGTH, strength - baseStr);
    }
    if (dexterity > baseDex) {
        character->addStatPoint(d2::game::StatType::DEXTERITY, dexterity - baseDex);
    }
    if (vitality > baseVit) {
        character->addStatPoint(d2::game::StatType::VITALITY, vitality - baseVit);
    }
    if (energy > baseEne) {
        character->addStatPoint(d2::game::StatType::ENERGY, energy - baseEne);
    }
    
    return character;
}

bool SaveManager::saveCharacterWithInventory(const d2::game::Character& character, 
                                             const d2::game::Inventory& inventory,
                                             const std::string& fileName) {
    // Create full path
    std::filesystem::path savePath = std::filesystem::path(m_saveDirectory) / fileName;
    
    // Open file for binary writing
    std::ofstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Create complete save data buffer
    std::vector<uint8_t> saveData;
    
    // First add the header (765 bytes)
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
    
    // Write stats at custom offsets (simplified for our implementation)
    // These offsets are in unused parts of the header
    // Strength at offset 100
    *reinterpret_cast<uint16_t*>(&header[100]) = static_cast<uint16_t>(character.getStrength());
    // Dexterity at offset 102
    *reinterpret_cast<uint16_t*>(&header[102]) = static_cast<uint16_t>(character.getDexterity());
    // Vitality at offset 104
    *reinterpret_cast<uint16_t*>(&header[104]) = static_cast<uint16_t>(character.getVitality());
    // Energy at offset 106
    *reinterpret_cast<uint16_t*>(&header[106]) = static_cast<uint16_t>(character.getEnergy());
    
    // Add header to save data
    saveData.insert(saveData.end(), header.begin(), header.end());
    
    // Now add item data
    // Item list starts with "JM" marker
    saveData.push_back('J');
    saveData.push_back('M');
    
    // Count items in inventory
    uint16_t itemCount = 0;
    std::vector<std::tuple<int, int, std::shared_ptr<d2::game::Item>>> uniqueItems;
    std::set<std::shared_ptr<d2::game::Item>> countedItems;
    
    for (int y = 0; y < inventory.getHeight(); ++y) {
        for (int x = 0; x < inventory.getWidth(); ++x) {
            auto item = inventory.getItemAt(x, y);
            if (item && countedItems.find(item) == countedItems.end()) {
                countedItems.insert(item);
                uniqueItems.push_back({x, y, item});
                itemCount++;
            }
        }
    }
    
    // Write item count
    saveData.push_back(itemCount & 0xFF);
    saveData.push_back((itemCount >> 8) & 0xFF);
    
    // Write item data for each unique item
    for (const auto& [x, y, item] : uniqueItems) {
        // Write item header "JM"
        saveData.push_back('J');
        saveData.push_back('M');
        
        // Write basic item data (simplified format)
        // Position
        saveData.push_back(static_cast<uint8_t>(x));
        saveData.push_back(static_cast<uint8_t>(y));
        
        // Item type
        saveData.push_back(static_cast<uint8_t>(item->getType()));
        
        // Item rarity
        saveData.push_back(static_cast<uint8_t>(item->getRarity()));
        
        // Item size
        saveData.push_back(static_cast<uint8_t>(item->getWidth()));
        saveData.push_back(static_cast<uint8_t>(item->getHeight()));
        
        // Item name length and name
        const std::string& name = item->getName();
        saveData.push_back(static_cast<uint8_t>(name.length()));
        saveData.insert(saveData.end(), name.begin(), name.end());
        
        // Combat stats
        uint16_t minDamage = static_cast<uint16_t>(item->getMinDamage());
        uint16_t maxDamage = static_cast<uint16_t>(item->getMaxDamage());
        uint16_t defense = static_cast<uint16_t>(item->getDefense());
        
        saveData.push_back(minDamage & 0xFF);
        saveData.push_back((minDamage >> 8) & 0xFF);
        saveData.push_back(maxDamage & 0xFF);
        saveData.push_back((maxDamage >> 8) & 0xFF);
        saveData.push_back(defense & 0xFF);
        saveData.push_back((defense >> 8) & 0xFF);
    }
    
    // Update file size in header
    uint32_t fileSize = static_cast<uint32_t>(saveData.size());
    *reinterpret_cast<uint32_t*>(&saveData[8]) = fileSize;
    
    // Calculate checksum over entire save data
    uint32_t checksum = calculateChecksum(saveData);
    
    // Write checksum back to save data
    *reinterpret_cast<uint32_t*>(&saveData[12]) = checksum;
    
    // Write entire save data to file
    file.write(reinterpret_cast<const char*>(saveData.data()), saveData.size());
    
    // Close file
    file.close();
    
    return true;
}

SaveManager::LoadResult SaveManager::loadCharacterWithInventory(const std::string& fileName) {
    LoadResult result;
    
    // Create full path
    std::filesystem::path savePath = std::filesystem::path(m_saveDirectory) / fileName;
    
    // Open file for binary reading
    std::ifstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        return result;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read entire file
    std::vector<uint8_t> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    file.close();
    
    if (fileData.size() < D2S_HEADER_SIZE) {
        return result;
    }
    
    // Verify signature
    uint32_t signature = *reinterpret_cast<uint32_t*>(&fileData[0]);
    if (signature != D2S_SIGNATURE) {
        return result;
    }
    
    // Extract stored checksum before validation
    uint32_t storedChecksum = *reinterpret_cast<uint32_t*>(&fileData[12]);
    
    // Calculate expected checksum
    uint32_t calculatedChecksum = calculateChecksum(fileData);
    
    // Validate checksum
    if (storedChecksum != calculatedChecksum) {
        return result;  // Invalid checksum - file is corrupted
    }
    
    // Read character class (offset 40)
    auto charClass = static_cast<d2::game::CharacterClass>(fileData[40]);
    
    // Read level (offset 43)
    uint8_t level = fileData[43];
    
    // Create character
    result.character = std::make_unique<d2::game::Character>(charClass);
    result.character->setLevel(level);
    
    // Create inventory
    result.inventory = std::make_unique<d2::game::Inventory>(10, 4);  // Standard size
    
    // Parse item data if present
    size_t pos = D2S_HEADER_SIZE;
    if (pos + 2 <= fileData.size() && fileData[pos] == 'J' && fileData[pos + 1] == 'M') {
        pos += 2;  // Skip JM marker
        
        // Read item count
        if (pos + 2 <= fileData.size()) {
            uint16_t itemCount = fileData[pos] | (fileData[pos + 1] << 8);
            pos += 2;
            
            // Read each item
            for (uint16_t i = 0; i < itemCount && pos < fileData.size(); ++i) {
                // Check for item header
                if (pos + 2 > fileData.size() || fileData[pos] != 'J' || fileData[pos + 1] != 'M') {
                    break;
                }
                pos += 2;  // Skip JM marker
                
                // Read item data
                if (pos + 8 > fileData.size()) {
                    break;
                }
                
                uint8_t x = fileData[pos++];
                uint8_t y = fileData[pos++];
                auto itemType = static_cast<d2::game::ItemType>(fileData[pos++]);
                auto rarity = static_cast<d2::game::ItemRarity>(fileData[pos++]);
                uint8_t width = fileData[pos++];
                uint8_t height = fileData[pos++];
                
                // Read item name
                if (pos >= fileData.size()) {
                    break;
                }
                uint8_t nameLength = fileData[pos++];
                if (pos + nameLength > fileData.size()) {
                    break;
                }
                std::string itemName(reinterpret_cast<const char*>(&fileData[pos]), nameLength);
                pos += nameLength;
                
                // Read combat stats
                if (pos + 6 > fileData.size()) {
                    break;
                }
                uint16_t minDamage = fileData[pos] | (fileData[pos + 1] << 8);
                pos += 2;
                uint16_t maxDamage = fileData[pos] | (fileData[pos + 1] << 8);
                pos += 2;
                uint16_t defense = fileData[pos] | (fileData[pos + 1] << 8);
                pos += 2;
                
                // Create item
                auto item = std::make_shared<d2::game::Item>(itemName, itemType);
                item->setRarity(rarity);
                item->setSize(width, height);
                
                if (itemType == d2::game::ItemType::WEAPON) {
                    item->setDamage(minDamage, maxDamage);
                } else if (itemType == d2::game::ItemType::ARMOR) {
                    item->setDefense(defense);
                }
                
                // Add to inventory
                result.inventory->addItem(item, x, y);
            }
        }
    }
    
    return result;
}

std::unique_ptr<d2::game::Character> SaveManager::loadCharacterFromBackup(const std::string& fileName) {
    // Create backup file path
    std::filesystem::path backupPath = std::filesystem::path(m_saveDirectory) / "backup" / (fileName + ".bak");
    
    // Check if backup exists
    if (!std::filesystem::exists(backupPath)) {
        return nullptr;
    }
    
    // Open backup file for binary reading
    std::ifstream file(backupPath, std::ios::binary);
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
    
    // Read stats from custom offsets
    uint16_t strength = *reinterpret_cast<uint16_t*>(&header[100]);
    uint16_t dexterity = *reinterpret_cast<uint16_t*>(&header[102]);
    uint16_t vitality = *reinterpret_cast<uint16_t*>(&header[104]);
    uint16_t energy = *reinterpret_cast<uint16_t*>(&header[106]);
    
    // Apply stats to character (need to subtract base stats first)
    // Get base stats for the class
    d2::game::Character tempChar(charClass);
    int baseStr = tempChar.getStrength();
    int baseDex = tempChar.getDexterity();
    int baseVit = tempChar.getVitality();
    int baseEne = tempChar.getEnergy();
    
    // Add the difference as stat points
    if (strength > baseStr) {
        character->addStatPoint(d2::game::StatType::STRENGTH, strength - baseStr);
    }
    if (dexterity > baseDex) {
        character->addStatPoint(d2::game::StatType::DEXTERITY, dexterity - baseDex);
    }
    if (vitality > baseVit) {
        character->addStatPoint(d2::game::StatType::VITALITY, vitality - baseVit);
    }
    if (energy > baseEne) {
        character->addStatPoint(d2::game::StatType::ENERGY, energy - baseEne);
    }
    
    return character;
}

} // namespace d2::save