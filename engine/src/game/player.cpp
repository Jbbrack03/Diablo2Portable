#include "game/player.h"

namespace d2::game {

Player::Player(Character&& character) 
    : m_character(std::move(character)) {
    position_ = glm::vec2(0.0f, 0.0f);
}

CharacterClass Player::getCharacterClass() const {
    return m_character.getCharacterClass();
}

void Player::move(const glm::vec2& delta) {
    position_ += delta;
}

} // namespace d2::game