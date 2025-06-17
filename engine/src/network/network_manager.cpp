#include "network/network_manager.h"

namespace d2::network {

bool NetworkManager::initialize() {
    // Minimal implementation to make test pass (Green phase)
    initialized_ = true;
    return true;
}

bool NetworkManager::isInitialized() const {
    return initialized_;
}

GameSession NetworkManager::hostGame(const std::string& gameName, uint32_t maxPlayers) {
    // Minimal implementation to make test pass (Green phase)
    if (!initialized_) {
        return GameSession{};
    }
    
    GameSession session;
    session.active_ = true;
    session.gameName_ = gameName;
    session.maxPlayers_ = maxPlayers;
    session.port_ = 6112; // Default Diablo II port
    
    return session;
}

GameSession NetworkManager::joinGame(const GameInfo& gameInfo) {
    // Minimal implementation to make test pass (Green phase)
    if (!initialized_) {
        return GameSession{};
    }
    
    GameSession session;
    session.active_ = true;
    session.gameName_ = gameInfo.name;
    session.port_ = gameInfo.port;
    
    return session;
}

std::vector<GameInfo> NetworkManager::discoverGames() {
    // Minimal implementation to make test pass (Green phase)
    return std::vector<GameInfo>{};
}

bool NetworkManager::broadcastGameQuery() {
    // Minimal implementation to make test pass (Green phase)
    return initialized_;
}

void NetworkManager::startDiscovery() {
    if (initialized_) {
        discovering_ = true;
    }
}

void NetworkManager::stopDiscovery() {
    discovering_ = false;
}

bool NetworkManager::isDiscovering() const {
    return discovering_;
}

uint16_t NetworkManager::getProtocolVersion() const {
    return 0x0100; // Diablo II protocol version 1.00
}

std::vector<uint8_t> NetworkManager::createJoinGamePacket(const std::string& playerName, CharacterClass characterClass) {
    // Minimal implementation to make test pass (Green phase)
    (void)playerName; // Suppress unused parameter warning
    (void)characterClass;
    
    std::vector<uint8_t> packet(28); // Expected D2 join packet size
    packet[0] = 0x68; // D2 join game packet ID
    return packet;
}

} // namespace d2::network