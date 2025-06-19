#include "game/player.h"

namespace d2::game {

Player::Player(const Character& character) 
    : m_character(character), m_position(0.0f, 0.0f) {
}

CharacterClass Player::getCharacterClass() const {
    return m_character.getCharacterClass();
}

glm::vec2 Player::getPosition() const {
    return m_position;
}

} // namespace d2::game