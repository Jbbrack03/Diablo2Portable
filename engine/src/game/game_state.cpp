#include "game/game_state.h"
#include "game/player.h"

namespace d2::game {

bool GameState::hasPlayer() const {
    return m_player != nullptr;
}

void GameState::setPlayer(std::shared_ptr<Player> player) {
    m_player = player;
}

std::shared_ptr<Player> GameState::getPlayer() const {
    return m_player;
}

} // namespace d2::game