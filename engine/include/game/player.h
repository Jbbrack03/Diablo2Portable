#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "game/character.h"

namespace d2::game {

class Player {
public:
    explicit Player(const Character& character);
    
    CharacterClass getCharacterClass() const;
    glm::vec2 getPosition() const;
    void setPosition(const glm::vec2& position);
    void move(const glm::vec2& delta);
    
private:
    Character m_character;
    glm::vec2 m_position;
};

} // namespace d2::game