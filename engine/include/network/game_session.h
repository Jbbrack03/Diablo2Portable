#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <algorithm>

namespace d2::network {

// Forward declaration
enum class CharacterClass : uint8_t;

struct PlayerInfo {
    uint32_t id;
    std::string name;
    CharacterClass characterClass;
};

enum class MessageType : uint8_t {
    PLAYER_MOVE = 0x01,
    PLAYER_ATTACK = 0x02,
    ITEM_PICKUP = 0x03,
    CHAT_MESSAGE = 0x04,
    GAME_STATE_UPDATE = 0x05
};

struct GameStateMessage {
    MessageType type;
    uint32_t playerId;
    glm::vec2 position;
    std::vector<uint8_t> data;
};

class GameSession {
    friend class NetworkManager;
public:
    GameSession() = default;
    ~GameSession() = default;

    bool isActive() const;
    
    // Game properties
    std::string getGameName() const;
    uint32_t getMaxPlayers() const;
    uint16_t getPort() const;
    
    // Player management
    bool addPlayer(const PlayerInfo& player);
    bool removePlayer(uint32_t playerId);
    uint32_t getPlayerCount() const;
    
    // Messaging
    bool sendMessage(const GameStateMessage& message);
    
    // Socket management (for real network implementation)
    int getSocketDescriptor() const;
    bool isListening() const;
    uint16_t getListeningPort() const;

private:
    bool active_ = false;
    std::string gameName_;
    uint32_t maxPlayers_ = 0;
    uint16_t port_ = 6112;
    std::vector<PlayerInfo> players_;
    int socketDescriptor_ = -1;
    bool listening_ = false;
};

} // namespace d2::network