#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace d2::network {

// Simple binary protocol for LAN multiplayer.
// Header: [type:1][length:2][sequence:1] = 4 bytes
// Body: variable length payload

enum class PacketType : uint8_t {
    DISCOVERY_QUERY    = 0x01,
    DISCOVERY_RESPONSE = 0x02,
    JOIN_REQUEST       = 0x03,
    JOIN_RESPONSE      = 0x04,
    POSITION_UPDATE    = 0x10,
    ATTACK             = 0x11,
    STATE_SYNC         = 0x20,
    CHAT               = 0x30,
    DISCONNECT         = 0xFF
};

struct PacketHeader {
    PacketType type;
    uint16_t length;   // payload length (excluding header)
    uint8_t sequence;
};

constexpr size_t PACKET_HEADER_SIZE = 4;
constexpr uint16_t D2_DEFAULT_PORT = 6112;

// Serialization helpers
std::vector<uint8_t> serializeHeader(const PacketHeader& header);
bool deserializeHeader(const uint8_t* data, size_t len, PacketHeader& out);

// Build complete packets (header + payload)
std::vector<uint8_t> buildDiscoveryQuery();
std::vector<uint8_t> buildDiscoveryResponse(const std::string& gameName, uint32_t currentPlayers, uint32_t maxPlayers);
std::vector<uint8_t> buildPositionUpdate(uint32_t playerId, const glm::vec2& position);
std::vector<uint8_t> buildAttackPacket(uint32_t playerId, uint32_t targetId, int32_t damage);
std::vector<uint8_t> buildChatPacket(uint32_t playerId, const std::string& message);

} // namespace d2::network
