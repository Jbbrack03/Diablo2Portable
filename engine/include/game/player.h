#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "game/character.h"
#include "game/entity.h"

namespace d2::game {

class Player : public Entity {
public:
    explicit Player(const Character& character);
    
    CharacterClass getCharacterClass() const;
    void move(const glm::vec2& delta);
    
private:
    Character m_character;
};

} // namespace d2::game