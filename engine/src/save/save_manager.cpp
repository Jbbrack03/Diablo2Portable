#include "game/save_manager.h"
#include "game/character.h"
#include <fstream>
#include <filesystem>
#include <cstring>

namespace d2::game {

namespace fs = std::filesystem;

SaveManager::SaveManager() {
}

SaveManager::~SaveManager() {
    // Stop auto-save thread if running
    if (auto_save_running) {
        auto_save_running = false;
        if (auto_save_thread.joinable()) {
            auto_save_thread.join();
        }
    }
}

bool SaveManager::saveCharacter(const Character& character, const std::string& path) {
    try {
        // Create directory if it doesn't exist
        fs::path filePath(path);
        if (filePath.has_parent_path()) {
            fs::create_directories(filePath.parent_path());
        }
        
        // Handle backups if enabled
        if (backup_enabled && fs::exists(path)) {
            rotateBackups(path);
        }
        
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write D2S header
        uint32_t signature = D2S_SIGNATURE;
        file.write(reinterpret_cast<const char*>(&signature), sizeof(signature));
        
        // Write version
        uint32_t version = D2S_VERSION;
        file.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        // Write file size (placeholder, will update later)
        uint32_t fileSize = 0;
        auto fileSizePos = file.tellp();
        file.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
        
        // Write checksum (placeholder)
        uint32_t checksum = 0;
        auto checksumPos = file.tellp();
        file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
        
        // Write character name (16 bytes, null-terminated)
        // Since Character doesn't have a name, use a placeholder
        char name[16] = "Player";
        file.write(name, 16);
        
        // Write character status flags
        uint8_t status = 0x00; // Normal character
        file.write(reinterpret_cast<const char*>(&status), sizeof(status));
        
        // Write character class
        uint8_t charClass = static_cast<uint8_t>(character.getCharacterClass());
        file.write(reinterpret_cast<const char*>(&charClass), sizeof(charClass));
        
        // Write level
        uint8_t level = static_cast<uint8_t>(character.getLevel());
        file.write(reinterpret_cast<const char*>(&level), sizeof(level));
        
        // Write experience (placeholder since Character doesn't expose getExperience)
        uint32_t experience = 0;
        file.write(reinterpret_cast<const char*>(&experience), sizeof(experience));
        
        // Write stats
        uint16_t strength = character.getStrength();
        uint16_t dexterity = character.getDexterity();
        uint16_t vitality = character.getVitality();
        uint16_t energy = character.getEnergy();
        
        file.write(reinterpret_cast<const char*>(&strength), sizeof(strength));
        file.write(reinterpret_cast<const char*>(&dexterity), sizeof(dexterity));
        file.write(reinterpret_cast<const char*>(&vitality), sizeof(vitality));
        file.write(reinterpret_cast<const char*>(&energy), sizeof(energy));
        
        // Character doesn't have inventory support in current implementation
        
        // Update file size
        auto endPos = file.tellp();
        fileSize = static_cast<uint32_t>(endPos);
        file.seekp(fileSizePos);
        file.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
        
        // Calculate and write checksum
        file.seekp(0, std::ios::end);
        file.close();
        
        // Reopen to calculate checksum
        checksum = calculateChecksum(path);
        file.open(path, std::ios::binary | std::ios::in | std::ios::out);
        file.seekp(checksumPos);
        file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
        file.close();
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::unique_ptr<Character> SaveManager::loadCharacter(const std::string& path) {
    if (!isValidSaveFile(path)) {
        return nullptr;
    }
    
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return nullptr;
        }
        
        // Skip signature and version (already validated)
        file.seekg(8);
        
        // Skip file size
        file.seekg(4, std::ios::cur);
        
        // Skip checksum (already validated)
        file.seekg(4, std::ios::cur);
        
        // Read character name
        char name[16];
        file.read(name, 16);
        name[15] = '\0'; // Ensure null termination
        
        // Skip status
        file.seekg(1, std::ios::cur);
        
        // Read character class
        uint8_t charClass;
        file.read(reinterpret_cast<char*>(&charClass), sizeof(charClass));
        
        // Create character
        auto character = std::make_unique<Character>(static_cast<CharacterClass>(charClass));
        // Character doesn't have setName, name is just stored in save file
        
        // Read level
        uint8_t level;
        file.read(reinterpret_cast<char*>(&level), sizeof(level));
        character->setLevel(level);
        
        // Read experience (but Character doesn't have setExperience)
        uint32_t experience;
        file.read(reinterpret_cast<char*>(&experience), sizeof(experience));
        // Can't set experience on character - API limitation
        
        // Read stats
        uint16_t strength, dexterity, vitality, energy;
        file.read(reinterpret_cast<char*>(&strength), sizeof(strength));
        file.read(reinterpret_cast<char*>(&dexterity), sizeof(dexterity));
        file.read(reinterpret_cast<char*>(&vitality), sizeof(vitality));
        file.read(reinterpret_cast<char*>(&energy), sizeof(energy));
        
        // Apply stat points (simplified - just set the difference)
        int strPoints = strength - character->getStrength();
        int dexPoints = dexterity - character->getDexterity();
        int vitPoints = vitality - character->getVitality();
        int enePoints = energy - character->getEnergy();
        
        if (strPoints > 0) character->addStatPoint(StatType::STRENGTH, strPoints);
        if (dexPoints > 0) character->addStatPoint(StatType::DEXTERITY, dexPoints);
        if (vitPoints > 0) character->addStatPoint(StatType::VITALITY, vitPoints);
        if (enePoints > 0) character->addStatPoint(StatType::ENERGY, enePoints);
        
        // Character doesn't have inventory support in current implementation
        
        return character;
    } catch (const std::exception&) {
        return nullptr;
    }
}

bool SaveManager::isValidSaveFile(const std::string& path) const {
    if (!fs::exists(path)) {
        return false;
    }
    
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Check signature
        uint32_t signature;
        file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
        if (signature != D2S_SIGNATURE) {
            return false;
        }
        
        // Check version
        uint32_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (version != D2S_VERSION) {
            return false;
        }
        
        // Verify checksum
        return verifyChecksum(path);
    } catch (const std::exception&) {
        return false;
    }
}

uint32_t SaveManager::calculateChecksum(const std::string& path) const {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return 0;
        }
        
        // Skip to after checksum field
        file.seekg(16); // Skip signature(4) + version(4) + filesize(4) + checksum(4)
        
        uint32_t checksum = 0;
        char byte;
        while (file.get(byte)) {
            checksum = (checksum << 1) + (checksum >> 31) + static_cast<uint8_t>(byte);
        }
        
        return checksum;
    } catch (const std::exception&) {
        return 0;
    }
}

bool SaveManager::verifyChecksum(const std::string& path) const {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Skip to checksum field
        file.seekg(12);
        
        // Read stored checksum
        uint32_t storedChecksum;
        file.read(reinterpret_cast<char*>(&storedChecksum), sizeof(storedChecksum));
        file.close();
        
        // Calculate actual checksum
        uint32_t actualChecksum = calculateChecksum(path);
        
        return storedChecksum == actualChecksum;
    } catch (const std::exception&) {
        return false;
    }
}

void SaveManager::registerCharacterForAutoSave(Character* character, const std::string& path) {
    if (!character) return;
    
    // Add to auto-save list
    auto_save_entries.push_back({character, path, std::chrono::steady_clock::now()});
    
    // Start auto-save thread if not running
    if (auto_save_enabled && !auto_save_running) {
        auto_save_running = true;
        auto_save_thread = std::thread(&SaveManager::runAutoSave, this);
    }
}

void SaveManager::runAutoSave() {
    while (auto_save_running && auto_save_enabled) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        auto now = std::chrono::steady_clock::now();
        for (auto& entry : auto_save_entries) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - entry.last_save).count();
            
            if (elapsed >= auto_save_interval) {
                if (saveCharacter(*entry.character, entry.path)) {
                    entry.last_save = now;
                }
            }
        }
    }
}

std::vector<std::string> SaveManager::getBackupFiles(const std::string& originalPath) const {
    std::vector<std::string> backups;
    
    for (int i = 1; i <= max_backups; i++) {
        std::string backupPath = generateBackupPath(originalPath, i);
        if (fs::exists(backupPath)) {
            backups.push_back(backupPath);
        }
    }
    
    return backups;
}

std::string SaveManager::generateBackupPath(const std::string& originalPath, int index) const {
    fs::path path(originalPath);
    std::string stem = path.stem().string();
    std::string ext = path.extension().string();
    std::string dir = path.parent_path().string();
    
    return dir + "/" + stem + ".backup" + std::to_string(index) + ext;
}

void SaveManager::rotateBackups(const std::string& path) {
    // Delete oldest backup if at max
    std::string oldestBackup = generateBackupPath(path, max_backups);
    if (fs::exists(oldestBackup)) {
        fs::remove(oldestBackup);
    }
    
    // Rotate existing backups
    for (int i = max_backups - 1; i >= 1; i--) {
        std::string currentBackup = generateBackupPath(path, i);
        std::string nextBackup = generateBackupPath(path, i + 1);
        
        if (fs::exists(currentBackup)) {
            fs::rename(currentBackup, nextBackup);
        }
    }
    
    // Copy current file to backup 1
    std::string firstBackup = generateBackupPath(path, 1);
    fs::copy_file(path, firstBackup, fs::copy_options::overwrite_existing);
}

} // namespace d2::game