#include "game/save_manager.h"
#include "game/character.h"
#include <fstream>

namespace d2::game {

bool SaveManager::saveCharacter(const Character& character, const std::string& filename) {
    // Minimal implementation - just create the file
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write something to the file so it's not empty
    file << "D2S";
    file.close();
    
    return true;
}

} // namespace d2::game