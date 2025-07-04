#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/dropped_item.h"
#include "map/map_loader.h"

namespace d2::game {

GameState::GameState() = default;

GameState::~GameState() = default;

bool GameState::hasPlayer() const {
    return m_player != nullptr;
}

void GameState::setPlayer(std::shared_ptr<Player> player) {
    m_player = player;
}

std::shared_ptr<Player> GameState::getPlayer() const {
    return m_player;
}

bool GameState::hasMap() const {
    return m_map != nullptr;
}

void GameState::setMap(std::unique_ptr<d2::map::Map> map) {
    m_map = std::move(map);
}

const d2::map::Map* GameState::getMap() const {
    return m_map.get();
}

EntityId GameState::addMonster(std::shared_ptr<Monster> monster) {
    EntityId id = m_entityManager.addEntity(monster);
    m_monsters[id] = monster;
    return id;
}

std::shared_ptr<Monster> GameState::getMonster(EntityId id) const {
    auto it = m_monsters.find(id);
    if (it != m_monsters.end()) {
        return it->second;
    }
    return nullptr;
}

size_t GameState::getMonsterCount() const {
    return m_monsters.size();
}

const std::unordered_map<EntityId, std::shared_ptr<Monster>>& GameState::getAllMonsters() const {
    return m_monsters;
}

EntityId GameState::addDroppedItem(std::shared_ptr<DroppedItem> item) {
    EntityId id = m_entityManager.addEntity(item);
    m_droppedItems[id] = item;
    return id;
}

std::shared_ptr<DroppedItem> GameState::getDroppedItem(EntityId id) const {
    auto it = m_droppedItems.find(id);
    if (it != m_droppedItems.end()) {
        return it->second;
    }
    return nullptr;
}

const std::unordered_map<EntityId, std::shared_ptr<DroppedItem>>& GameState::getAllDroppedItems() const {
    return m_droppedItems;
}

void GameState::removeDroppedItem(EntityId id) {
    m_droppedItems.erase(id);
}

} // namespace d2::game