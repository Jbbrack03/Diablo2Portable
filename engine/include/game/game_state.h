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
class DroppedItem;

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
    
    // Dropped item management
    EntityId addDroppedItem(std::shared_ptr<DroppedItem> item);
    std::shared_ptr<DroppedItem> getDroppedItem(EntityId id) const;
    const std::unordered_map<EntityId, std::shared_ptr<DroppedItem>>& getAllDroppedItems() const;
    void removeDroppedItem(EntityId id);

private:
    std::shared_ptr<Player> m_player;
    std::unique_ptr<d2::map::Map> m_map;
    std::unordered_map<EntityId, std::shared_ptr<Monster>> m_monsters;
    std::unordered_map<EntityId, std::shared_ptr<DroppedItem>> m_droppedItems;
    EntityManager m_entityManager;
};

} // namespace d2::game