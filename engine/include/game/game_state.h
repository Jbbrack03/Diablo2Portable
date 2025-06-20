#pragma once

#include <memory>

namespace d2::map {
    class Map;
}

namespace d2::game {

class Player;

class GameState {
public:
    GameState();
    ~GameState();
    
    bool hasPlayer() const;
    void setPlayer(std::shared_ptr<Player> player);
    std::shared_ptr<Player> getPlayer() const;
    
    bool hasMap() const;
    void setMap(std::unique_ptr<d2::map::Map> map);
    const d2::map::Map* getMap() const;

private:
    std::shared_ptr<Player> m_player;
    std::unique_ptr<d2::map::Map> m_map;
};

} // namespace d2::game