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

void Player::setPosition(const glm::vec2& position) {
    m_position = position;
}

void Player::move(const glm::vec2& delta) {
    m_position += delta;
}

} // namespace d2::game