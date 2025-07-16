#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "game/character.h"
#include "game/entity.h"

namespace d2::game {

class Player : public Entity {
public:
    explicit Player(Character&& character);
    
    // Temporary constructor for backward compatibility with tests
    // Takes a copy to avoid breaking existing code
    template<typename T>
    explicit Player(const T& characterRef) 
        : m_character(Character(characterRef.getCharacterClass())) {
        position_ = glm::vec2(0.0f, 0.0f);
    }
    
    CharacterClass getCharacterClass() const;
    void move(const glm::vec2& delta);
    
private:
    Character m_character;
};

} // namespace d2::game