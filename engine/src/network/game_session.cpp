#include "network/game_session.h"
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

namespace d2::network {

GameSession::~GameSession() {
    // Clean up socket if it's open
    if (socketDescriptor_ > 0) {
        close(socketDescriptor_);
        socketDescriptor_ = -1;
    }
}

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

int GameSession::getSocketDescriptor() const {
    return socketDescriptor_;
}

bool GameSession::isListening() const {
    return listening_;
}

uint16_t GameSession::getListeningPort() const {
    return port_;
}

bool GameSession::sendRawData(const std::vector<uint8_t>& data) {
    // GREEN phase - implement minimal socket send to pass test
    if (socketDescriptor_ <= 0 || !active_ || data.empty()) {
        return false;
    }
    
    // For testing purposes, we'll simulate sending data
    // In a real implementation, we'd send to connected clients
    // For now, just track that we "sent" the data
    bytesSent_ += data.size();
    
    // In a host session without clients, we can consider the send successful
    // Real implementation would use send() or sendto() with actual connected sockets
    return true;
}

size_t GameSession::getBytesSent() const {
    return bytesSent_;
}

} // namespace d2::network