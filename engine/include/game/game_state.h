#pragma once

#include <memory>
#include <unordered_map>
#include "game/entity_manager.h"

namespace d2::map {
    class Map;
}

namespace d2::game {

class Player;
class Monster;

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
    
    // Monster management
    EntityId addMonster(std::shared_ptr<Monster> monster);
    std::shared_ptr<Monster> getMonster(EntityId id) const;
    size_t getMonsterCount() const;
    const std::unordered_map<EntityId, std::shared_ptr<Monster>>& getAllMonsters() const;

private:
    std::shared_ptr<Player> m_player;
    std::unique_ptr<d2::map::Map> m_map;
    std::unordered_map<EntityId, std::shared_ptr<Monster>> m_monsters;
    EntityManager m_entityManager;
};

} // namespace d2::game