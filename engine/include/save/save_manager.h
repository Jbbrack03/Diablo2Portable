#pragma once

#include <string>
#include <memory>

namespace d2::game {
    class Character;
    class Inventory;
}

namespace d2::save {

class SaveManager {
public:
    explicit SaveManager(const std::string& saveDirectory);
    
    const std::string& getSaveDirectory() const { return m_saveDirectory; }
    
    // Save/load character
    bool saveCharacter(const d2::game::Character& character, const std::string& fileName);
    bool saveCharacterWithInventory(const d2::game::Character& character, 
                                    const d2::game::Inventory& inventory,
                                    const std::string& fileName);
    std::unique_ptr<d2::game::Character> loadCharacter(const std::string& fileName);
    
    // Result structure for loading character with inventory
    struct LoadResult {
        std::unique_ptr<d2::game::Character> character;
        std::unique_ptr<d2::game::Inventory> inventory;
    };
    
    LoadResult loadCharacterWithInventory(const std::string& fileName);
    
    // Backup operations
    std::unique_ptr<d2::game::Character> loadCharacterFromBackup(const std::string& fileName);
    
private:
    std::string m_saveDirectory;
};

} // namespace d2::save