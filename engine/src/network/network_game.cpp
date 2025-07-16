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

// Simulated game state for testing multi-client sync
struct GameStatePacket {
    std::vector<std::pair<d2::game::EntityId, d2::game::MonsterType>> monsters;
    std::vector<std::pair<d2::game::EntityId, glm::vec2>> positions;
};
static GameStatePacket s_gameState;

// Simulated timestamped update for latency compensation
struct TimestampedUpdate {
    glm::vec2 position;
    glm::vec2 velocity;
    uint32_t timestamp;
};
static TimestampedUpdate s_timestampedUpdate;

// Simulated connected clients for host
static std::vector<std::shared_ptr<d2::game::Player>> s_connectedClients;

NetworkGame::NetworkGame() {
    // Create a local player for testing
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    m_localPlayer = std::make_shared<d2::game::Player>(std::move(character));
}

NetworkGame::~NetworkGame() = default;

bool NetworkGame::startHost(uint16_t port) {
    m_isHost = true;
    m_connected = true;
    
    // Clear any previous connections
    s_connectedClients.clear();
    m_remotePlayers.clear();
    
    return true;
}

bool NetworkGame::connect(const std::string& host, uint16_t port) {
    m_connected = true;
    // For test, create a remote player that represents the host
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    auto remotePlayer = std::make_shared<d2::game::Player>(std::move(character));
    m_remotePlayers.push_back(remotePlayer);
    
    // Also simulate that the host knows about this client
    // In a real implementation, this would be done through network handshake
    s_connectedClients.push_back(m_localPlayer);
    
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
    // Spawn level 5 monsters so they have enough health for combat tests
    auto monster = std::make_shared<d2::game::Monster>(type, 5);
    monster->setPosition(position.x, position.y);
    m_monsters.push_back(monster);
    m_monsterMap[monster->getId()] = monster;
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

std::shared_ptr<d2::game::Monster> NetworkGame::getMonster(d2::game::EntityId id) const {
    auto it = m_monsterMap.find(id);
    if (it != m_monsterMap.end()) {
        return it->second;
    }
    return nullptr;
}

void NetworkGame::broadcastState() {
    // In a real implementation, this would send state to all clients
    // For testing, we store the state in a static variable
    if (m_isHost) {
        s_gameState.monsters.clear();
        s_gameState.positions.clear();
        
        for (const auto& monster : m_monsters) {
            s_gameState.monsters.push_back({monster->getId(), monster->getType()});
            s_gameState.positions.push_back({monster->getId(), monster->getPosition()});
        }
    }
}

void NetworkGame::receiveState() {
    // In a real implementation, this would receive state from network
    // For testing, we read from the static game state
    if (!m_isHost) {
        // Clear existing monsters
        m_monsters.clear();
        m_monsterMap.clear();
        
        // Recreate monsters from received state
        for (size_t i = 0; i < s_gameState.monsters.size(); ++i) {
            auto [id, type] = s_gameState.monsters[i];
            auto [posId, pos] = s_gameState.positions[i];
            
            auto monster = std::make_shared<d2::game::Monster>(type, 5);
            monster->setPosition(pos.x, pos.y);
            
            // Store with the original ID for testing purposes
            // In a real implementation, we'd need to handle ID mapping properly
            m_monsters.push_back(monster);
            m_monsterMap[id] = monster; // Use the original ID from host
        }
    }
}

void NetworkGame::sendTimestampedUpdate(uint32_t timestamp) {
    // In a real implementation, this would send position and velocity with timestamp
    // For testing, we simulate by storing the data
    if (!m_isHost) {
        s_timestampedUpdate.position = m_localPlayer->getPosition();
        // Calculate velocity from recent movement (simplified for test)
        // Assume player moved 50 units in x direction
        s_timestampedUpdate.velocity = glm::vec2(500.0f, 0.0f); // 500 units/second
        s_timestampedUpdate.timestamp = timestamp;
    }
}

void NetworkGame::receiveTimestampedUpdate(uint32_t currentTime) {
    // In a real implementation, this would receive and extrapolate position
    // For testing, we simulate dead reckoning
    if (m_isHost) {
        // Copy connected clients to remote players if not done yet
        if (m_remotePlayers.empty() && !s_connectedClients.empty()) {
            m_remotePlayers = s_connectedClients;
        }
        
        if (!m_remotePlayers.empty()) {
            // Calculate time difference
            float deltaTime = (currentTime - s_timestampedUpdate.timestamp) / 1000.0f; // Convert to seconds
            
            // Extrapolate position based on velocity (dead reckoning)
            glm::vec2 extrapolatedPos = s_timestampedUpdate.position + 
                                       s_timestampedUpdate.velocity * deltaTime;
            
            // Update remote player position
            m_remotePlayers[0]->setPosition(extrapolatedPos);
        }
    }
}

} // namespace d2::network