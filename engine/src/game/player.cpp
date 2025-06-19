#include "game/player.h"

namespace d2::game {

Player::Player(const Character& character) 
    : m_character(character) {
}

CharacterClass Player::getCharacterClass() const {
    return m_character.getCharacterClass();
}

} // namespace d2::game