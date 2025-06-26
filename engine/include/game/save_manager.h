#pragma once

#include <string>
#include <memory>

namespace d2::game {

// Forward declaration
class Character;

// SaveManager class - saves characters to D2S format
class SaveManager {
public:
    SaveManager() = default;
    
    // Save a character to a file
    bool saveCharacter(const Character& character, const std::string& filename);
    
    // Load a character from a file
    std::unique_ptr<Character> loadCharacter(const std::string& filename);
};

} // namespace d2::game