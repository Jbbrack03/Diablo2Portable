#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace d2::game {
    class Player;
    class Monster;
    enum class MonsterType;
    using EntityId = uint32_t;
}

namespace d2::network {

class NetworkGame {
public:
    NetworkGame();
    ~NetworkGame();
    
    bool startHost(uint16_t port);
    bool connect(const std::string& host, uint16_t port);
    
    std::shared_ptr<d2::game::Player> getPlayer() const;
    void sendUpdate();
    void receiveUpdate();
    
    std::shared_ptr<d2::game::Player> getRemotePlayer(size_t index) const;
    
    // Monster management
    std::shared_ptr<d2::game::Monster> spawnMonster(d2::game::MonsterType type, const glm::vec2& position);
    void sendAttack(d2::game::EntityId targetId, int damage);
    
private:
    std::shared_ptr<d2::game::Player> m_localPlayer;
    std::vector<std::shared_ptr<d2::game::Player>> m_remotePlayers;
    std::vector<std::shared_ptr<d2::game::Monster>> m_monsters;
    bool m_isHost = false;
    bool m_connected = false;
    int m_lastDamage = 0;
    d2::game::EntityId m_lastTargetId = 0;
};

} // namespace d2::network