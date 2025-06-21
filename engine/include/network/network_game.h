#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace d2::game {
    class Player;
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
    
private:
    std::shared_ptr<d2::game::Player> m_localPlayer;
    std::vector<std::shared_ptr<d2::game::Player>> m_remotePlayers;
    bool m_isHost = false;
    bool m_connected = false;
};

} // namespace d2::network