#include "network/d2_protocol.h"
#include <cstring>

namespace d2::network {

std::vector<uint8_t> serializeHeader(const PacketHeader& header) {
    std::vector<uint8_t> data(PACKET_HEADER_SIZE);
    data[0] = static_cast<uint8_t>(header.type);
    data[1] = static_cast<uint8_t>(header.length & 0xFF);
    data[2] = static_cast<uint8_t>((header.length >> 8) & 0xFF);
    data[3] = header.sequence;
    return data;
}

bool deserializeHeader(const uint8_t* data, size_t len, PacketHeader& out) {
    if (!data || len < PACKET_HEADER_SIZE) return false;
    out.type = static_cast<PacketType>(data[0]);
    out.length = static_cast<uint16_t>(data[1]) | (static_cast<uint16_t>(data[2]) << 8);
    out.sequence = data[3];
    return true;
}

static void appendU32(std::vector<uint8_t>& v, uint32_t val) {
    v.push_back(static_cast<uint8_t>(val & 0xFF));
    v.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    v.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    v.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
}

static void appendFloat(std::vector<uint8_t>& v, float val) {
    uint32_t bits;
    std::memcpy(&bits, &val, sizeof(bits));
    appendU32(v, bits);
}

std::vector<uint8_t> buildDiscoveryQuery() {
    PacketHeader hdr{PacketType::DISCOVERY_QUERY, 0, 0};
    return serializeHeader(hdr);
}

std::vector<uint8_t> buildDiscoveryResponse(const std::string& gameName, uint32_t currentPlayers, uint32_t maxPlayers) {
    std::vector<uint8_t> payload;
    appendU32(payload, currentPlayers);
    appendU32(payload, maxPlayers);
    // Append game name as length-prefixed string
    appendU32(payload, static_cast<uint32_t>(gameName.size()));
    payload.insert(payload.end(), gameName.begin(), gameName.end());

    PacketHeader hdr{PacketType::DISCOVERY_RESPONSE, static_cast<uint16_t>(payload.size()), 0};
    auto packet = serializeHeader(hdr);
    packet.insert(packet.end(), payload.begin(), payload.end());
    return packet;
}

std::vector<uint8_t> buildPositionUpdate(uint32_t playerId, const glm::vec2& position) {
    std::vector<uint8_t> payload;
    appendU32(payload, playerId);
    appendFloat(payload, position.x);
    appendFloat(payload, position.y);

    PacketHeader hdr{PacketType::POSITION_UPDATE, static_cast<uint16_t>(payload.size()), 0};
    auto packet = serializeHeader(hdr);
    packet.insert(packet.end(), payload.begin(), payload.end());
    return packet;
}

std::vector<uint8_t> buildAttackPacket(uint32_t playerId, uint32_t targetId, int32_t damage) {
    std::vector<uint8_t> payload;
    appendU32(payload, playerId);
    appendU32(payload, targetId);
    appendU32(payload, static_cast<uint32_t>(damage));

    PacketHeader hdr{PacketType::ATTACK, static_cast<uint16_t>(payload.size()), 0};
    auto packet = serializeHeader(hdr);
    packet.insert(packet.end(), payload.begin(), payload.end());
    return packet;
}

std::vector<uint8_t> buildChatPacket(uint32_t playerId, const std::string& message) {
    std::vector<uint8_t> payload;
    appendU32(payload, playerId);
    appendU32(payload, static_cast<uint32_t>(message.size()));
    payload.insert(payload.end(), message.begin(), message.end());

    PacketHeader hdr{PacketType::CHAT, static_cast<uint16_t>(payload.size()), 0};
    auto packet = serializeHeader(hdr);
    packet.insert(packet.end(), payload.begin(), payload.end());
    return packet;
}

} // namespace d2::network
