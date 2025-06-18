#pragma once

#include <string>

namespace d2::game {
    class Character;
}

namespace d2::save {

class SaveManager {
public:
    explicit SaveManager(const std::string& saveDirectory);
    
    const std::string& getSaveDirectory() const { return m_saveDirectory; }
    
    // Save/load character
    bool saveCharacter(const d2::game::Character& character, const std::string& fileName);
    
private:
    std::string m_saveDirectory;
};

} // namespace d2::save