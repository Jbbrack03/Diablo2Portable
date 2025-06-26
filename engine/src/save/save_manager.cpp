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
    
    // Write D2S signature
    uint32_t signature = 0xAA55AA55;
    file.write(reinterpret_cast<const char*>(&signature), sizeof(signature));
    
    file.close();
    
    return true;
}

} // namespace d2::game