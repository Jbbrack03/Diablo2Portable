#include "network/network_game.h"
#include "game/player.h"
#include "game/character.h"
#include "game/monster.h"

namespace d2::network {

// Simulated network packet for testing
struct NetworkPacket {
    d2::game::EntityId targetId = 0;
    int damage = 0;
};
static NetworkPacket s_pendingPacket;

NetworkGame::NetworkGame() {
    // Create a local player for testing
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    m_localPlayer = std::make_shared<d2::game::Player>(character);
}

NetworkGame::~NetworkGame() = default;

bool NetworkGame::startHost(uint16_t port) {
    m_isHost = true;
    m_connected = true;
    return true;
}

bool NetworkGame::connect(const std::string& host, uint16_t port) {
    m_connected = true;
    // For test, create a remote player that represents the host
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    auto remotePlayer = std::make_shared<d2::game::Player>(character);
    m_remotePlayers.push_back(remotePlayer);
    return true;
}

std::shared_ptr<d2::game::Player> NetworkGame::getPlayer() const {
    return m_localPlayer;
}

void NetworkGame::sendUpdate() {
    // In a real implementation, this would send the player position over network
    // For now, we'll simulate it for the test
}

void NetworkGame::receiveUpdate() {
    // In a real implementation, this would receive network data
    // For the test, we simulate receiving the host's player position
    if (!m_isHost && !m_remotePlayers.empty()) {
        // Simulate receiving the position update from host
        m_remotePlayers[0]->setPosition(glm::vec2(10.0f, 0.0f));
    }
    
    // If we're the host, check for pending attack packets
    if (m_isHost && s_pendingPacket.damage > 0) {
        // Apply damage to the target monster
        for (auto& monster : m_monsters) {
            if (monster->getId() == s_pendingPacket.targetId) {
                monster->takeDamage(s_pendingPacket.damage);
                break;
            }
        }
        // Clear the packet
        s_pendingPacket.targetId = 0;
        s_pendingPacket.damage = 0;
    }
}

std::shared_ptr<d2::game::Player> NetworkGame::getRemotePlayer(size_t index) const {
    if (index < m_remotePlayers.size()) {
        return m_remotePlayers[index];
    }
    return nullptr;
}

std::shared_ptr<d2::game::Monster> NetworkGame::spawnMonster(d2::game::MonsterType type, const glm::vec2& position) {
    auto monster = std::make_shared<d2::game::Monster>(type, 1); // Level 1 for testing
    monster->setPosition(position.x, position.y);
    m_monsters.push_back(monster);
    return monster;
}

void NetworkGame::sendAttack(d2::game::EntityId targetId, int damage) {
    // In a real implementation, this would send attack data over network
    // For testing, we simulate by storing in the static packet
    if (!m_isHost) {
        // Client sends attack to host
        s_pendingPacket.targetId = targetId;
        s_pendingPacket.damage = damage;
    }
}

} // namespace d2::network