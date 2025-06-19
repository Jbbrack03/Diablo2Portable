#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "game/character.h"

namespace d2::game {

class Player {
public:
    explicit Player(const Character& character);
    
    CharacterClass getCharacterClass() const;
    
private:
    Character m_character;
};

} // namespace d2::game