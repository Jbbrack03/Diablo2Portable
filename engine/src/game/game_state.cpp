#include "game/game_state.h"
#include "game/player.h"
#include "map/map_loader.h"

namespace d2::game {

GameState::GameState() = default;

GameState::~GameState() = default;

bool GameState::hasPlayer() const {
    return m_player != nullptr;
}

void GameState::setPlayer(std::shared_ptr<Player> player) {
    m_player = player;
}

std::shared_ptr<Player> GameState::getPlayer() const {
    return m_player;
}

bool GameState::hasMap() const {
    return m_map != nullptr;
}

void GameState::setMap(std::unique_ptr<d2::map::Map> map) {
    m_map = std::move(map);
}

const d2::map::Map* GameState::getMap() const {
    return m_map.get();
}

} // namespace d2::game