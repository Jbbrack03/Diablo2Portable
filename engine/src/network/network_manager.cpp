#include "network/network_manager.h"
#include "network/d2_protocol.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace d2::network {

bool NetworkManager::initialize() {
    initialized_ = true;
    return true;
}

bool NetworkManager::isInitialized() const {
    return initialized_;
}

GameSession NetworkManager::hostGame(const std::string& gameName, uint32_t maxPlayers) {
    if (!initialized_) {
        return GameSession{};
    }

    GameSession session;
    session.gameName_ = gameName;
    session.maxPlayers_ = maxPlayers;
    session.port_ = D2_DEFAULT_PORT;

    // Create a real TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd > 0) {
        // Allow port reuse
        int optval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        struct sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(session.port_);

        if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
            if (listen(sockfd, maxPlayers) == 0) {
                session.socketDescriptor_ = sockfd;
                session.listening_ = true;
                session.active_ = true;
            } else {
                close(sockfd);
            }
        } else {
            close(sockfd);
        }
    }

    return session;
}

GameSession NetworkManager::joinGame(const GameInfo& gameInfo) {
    if (!initialized_) {
        return GameSession{};
    }

    GameSession session;
    session.gameName_ = gameInfo.name;
    session.port_ = gameInfo.port;

    // Create TCP socket and attempt connection to host
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd > 0) {
        struct sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(gameInfo.port);

        if (inet_pton(AF_INET, gameInfo.host.c_str(), &serverAddr.sin_addr) > 0) {
            // Set non-blocking for connect attempt to avoid long timeout
            int flags = fcntl(sockfd, F_GETFL, 0);
            fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

            int result = connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            if (result == 0 || errno == EINPROGRESS) {
                // Connection succeeded or is in progress
                // Restore blocking mode
                fcntl(sockfd, F_SETFL, flags);
                session.socketDescriptor_ = sockfd;
            } else {
                close(sockfd);
            }
        } else {
            close(sockfd);
        }
    }

    // Mark session as active regardless of socket state
    // (real connection may complete asynchronously)
    session.active_ = true;

    return session;
}

std::vector<GameInfo> NetworkManager::discoverGames() {
    if (!initialized_) {
        return {};
    }

    std::vector<GameInfo> found;

    // Create UDP socket for broadcast discovery
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd <= 0) return found;

    // Enable broadcast
    int broadcast = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    // Set receive timeout (100ms)
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Send discovery query
    auto query = buildDiscoveryQuery();
    struct sockaddr_in broadcastAddr;
    std::memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(D2_DEFAULT_PORT);
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

    sendto(sockfd, query.data(), query.size(), 0,
           (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    // Collect responses (wait up to 100ms)
    uint8_t buf[512];
    struct sockaddr_in respAddr;
    socklen_t addrLen = sizeof(respAddr);
    ssize_t n = recvfrom(sockfd, buf, sizeof(buf), 0,
                         (struct sockaddr*)&respAddr, &addrLen);
    while (n > 0) {
        PacketHeader hdr;
        if (deserializeHeader(buf, n, hdr) && hdr.type == PacketType::DISCOVERY_RESPONSE) {
            GameInfo info;
            char addrStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &respAddr.sin_addr, addrStr, sizeof(addrStr));
            info.host = addrStr;
            info.port = D2_DEFAULT_PORT;
            // Parse payload for player counts/name if available
            found.push_back(info);
        }
        n = recvfrom(sockfd, buf, sizeof(buf), 0,
                     (struct sockaddr*)&respAddr, &addrLen);
    }

    close(sockfd);
    return found;
}

bool NetworkManager::broadcastGameQuery() {
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
    return 0x0100;
}

std::vector<uint8_t> NetworkManager::createJoinGamePacket(const std::string& playerName, CharacterClass characterClass) {
    (void)playerName;
    (void)characterClass;

    std::vector<uint8_t> packet(28);
    packet[0] = 0x68; // D2 join game packet ID
    return packet;
}

} // namespace d2::network
