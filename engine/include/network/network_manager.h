#pragma once

#include <string>
#include <vector>
#include <memory>
#include "game_session.h"

namespace d2::network {

struct GameInfo {
    std::string name;
    std::string host;
    uint16_t port;
    uint32_t currentPlayers;
    uint32_t maxPlayers;
};

enum class CharacterClass : uint8_t {
    AMAZON = 0,
    SORCERESS = 1,
    NECROMANCER = 2,
    PALADIN = 3,
    BARBARIAN = 4,
    DRUID = 5,
    ASSASSIN = 6
};

class NetworkManager {
public:
    NetworkManager() = default;
    ~NetworkManager() = default;

    bool initialize();
    bool isInitialized() const;
    
    // Game hosting
    GameSession hostGame(const std::string& gameName, uint32_t maxPlayers);
    GameSession joinGame(const GameInfo& gameInfo);
    
    // Game discovery
    std::vector<GameInfo> discoverGames();
    bool broadcastGameQuery();
    void startDiscovery();
    void stopDiscovery();
    bool isDiscovering() const;
    
    // Protocol compatibility
    uint16_t getProtocolVersion() const;
    std::vector<uint8_t> createJoinGamePacket(const std::string& playerName, CharacterClass characterClass);

private:
    bool initialized_ = false;
    bool discovering_ = false;
};

} // namespace d2::network