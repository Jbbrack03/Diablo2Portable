#include <gtest/gtest.h>
#include <memory>
#include "game/entity_manager.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/character.h"

using namespace d2::game;

class EntityManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<EntityManager>();
    }
    
    std::unique_ptr<EntityManager> manager;
};

// Test 1: EntityManager should be able to add entities
TEST_F(EntityManagerTest, AddEntity) {
    Character character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    
    EntityId id = manager->addEntity(player);
    
    EXPECT_NE(id, INVALID_ENTITY_ID);
    EXPECT_EQ(manager->getEntityCount(), 1);
}