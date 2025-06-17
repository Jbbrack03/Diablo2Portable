#include "network/game_session.h"

namespace d2::network {

bool GameSession::isActive() const {
    return active_;
}

std::string GameSession::getGameName() const {
    return gameName_;
}

uint32_t GameSession::getMaxPlayers() const {
    return maxPlayers_;
}

uint16_t GameSession::getPort() const {
    return port_;
}

bool GameSession::addPlayer(const PlayerInfo& player) {
    // Minimal implementation to make test pass (Green phase)
    if (players_.size() >= maxPlayers_) {
        return false;
    }
    
    players_.push_back(player);
    return true;
}

bool GameSession::removePlayer(uint32_t playerId) {
    // Minimal implementation to make test pass (Green phase)
    auto it = std::find_if(players_.begin(), players_.end(),
        [playerId](const PlayerInfo& player) {
            return player.id == playerId;
        });
    
    if (it != players_.end()) {
        players_.erase(it);
        return true;
    }
    
    return false;
}

uint32_t GameSession::getPlayerCount() const {
    return static_cast<uint32_t>(players_.size());
}

bool GameSession::sendMessage(const GameStateMessage& message) {
    // Minimal implementation to make test pass (Green phase)
    (void)message; // Suppress unused parameter warning
    return active_;
}

} // namespace d2::network