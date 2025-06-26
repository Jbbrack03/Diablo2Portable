#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <cstdint>

namespace d2::game {

class Character;

// D2S save file format manager
// Handles saving and loading character data in Diablo II save format
class SaveManager {
public:
    SaveManager();
    ~SaveManager();
    
    // Check if save manager is ready
    bool isReady() const { return true; }
    
    // Save character to D2S file
    bool saveCharacter(const Character& character, const std::string& path);
    
    // Load character from D2S file
    std::unique_ptr<Character> loadCharacter(const std::string& path);
    
    // Validate save file
    bool isValidSaveFile(const std::string& path) const;
    
    // Checksum operations
    uint32_t calculateChecksum(const std::string& path) const;
    bool verifyChecksum(const std::string& path) const;
    
    // Auto-save functionality
    void setAutoSaveEnabled(bool enabled) { auto_save_enabled = enabled; }
    void setAutoSaveInterval(int seconds) { auto_save_interval = seconds; }
    void registerCharacterForAutoSave(Character* character, const std::string& path);
    
    // Backup functionality
    void setBackupEnabled(bool enabled) { backup_enabled = enabled; }
    void setMaxBackups(int count) { max_backups = count; }
    std::vector<std::string> getBackupFiles(const std::string& originalPath) const;
    
private:
    // D2S file constants
    static constexpr uint32_t D2S_SIGNATURE = 0xAA55AA55;
    static constexpr uint32_t D2S_VERSION = 0x60; // Version 96
    
    // Auto-save state
    std::atomic<bool> auto_save_enabled{false};
    std::atomic<int> auto_save_interval{300}; // 5 minutes default
    std::thread auto_save_thread;
    std::atomic<bool> auto_save_running{false};
    
    // Backup settings
    bool backup_enabled{false};
    int max_backups{3};
    
    // Helper methods
    void runAutoSave();
    std::string generateBackupPath(const std::string& originalPath, int index) const;
    void rotateBackups(const std::string& path);
    
    // Registered characters for auto-save
    struct AutoSaveEntry {
        Character* character;
        std::string path;
        std::chrono::steady_clock::time_point last_save;
    };
    std::vector<AutoSaveEntry> auto_save_entries;
};

} // namespace d2::game