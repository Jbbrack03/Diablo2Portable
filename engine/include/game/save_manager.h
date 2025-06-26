#pragma once

#include <string>

namespace d2::game {

// Forward declaration
class Character;

// SaveManager class - saves characters to D2S format
class SaveManager {
public:
    SaveManager() = default;
    
    // Save a character to a file
    bool saveCharacter(const Character& character, const std::string& filename);
};

} // namespace d2::game