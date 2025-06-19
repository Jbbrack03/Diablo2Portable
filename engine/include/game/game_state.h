#pragma once

#include <memory>

namespace d2::game {

class Player;

class GameState {
public:
    GameState() = default;
    
    bool hasPlayer() const;
    void setPlayer(std::shared_ptr<Player> player);
    std::shared_ptr<Player> getPlayer() const;

private:
    std::shared_ptr<Player> m_player;
};

} // namespace d2::game