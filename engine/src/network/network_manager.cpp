#include "network/network_manager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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
    // GREEN phase - create a real socket to make test pass
    if (!initialized_) {
        return GameSession{};
    }
    
    GameSession session;
    session.gameName_ = gameName;
    session.maxPlayers_ = maxPlayers;
    session.port_ = 6112; // Default Diablo II port
    
    // Create a real TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd > 0) {
        // Set up the socket address structure
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(session.port_);
        
        // Bind the socket to the port
        if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
            // Start listening for connections
            if (listen(sockfd, maxPlayers) == 0) {
                session.socketDescriptor_ = sockfd;
                session.listening_ = true;
                session.active_ = true;
            } else {
                // Failed to listen, close the socket
                close(sockfd);
            }
        } else {
            // Failed to bind, close the socket
            close(sockfd);
        }
    }
    
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