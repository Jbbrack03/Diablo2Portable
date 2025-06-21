#include <gtest/gtest.h>
#include <memory>
#include "network/network_game.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/character.h"

using namespace d2::network;
using namespace d2::game;

class NetworkGameTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
};

// STEP 1: Write exactly ONE failing test
TEST_F(NetworkGameTest, SynchronizePlayers) {
    NetworkGame host;
    NetworkGame client;
    
    host.startHost(8999);
    client.connect("localhost", 8999);
    
    // Host moves player
    host.getPlayer()->move(glm::vec2(10, 0));
    host.sendUpdate();
    
    // Client receives update
    client.receiveUpdate();
    auto remotePlayer = client.getRemotePlayer(0);
    
    EXPECT_FLOAT_EQ(remotePlayer->getPosition().x, 10.0f);
}

// STEP 2: Write next failing test for combat synchronization
TEST_F(NetworkGameTest, SynchronizeDamage) {
    NetworkGame host;
    NetworkGame client;
    
    // Setup connection
    host.startHost(8999);
    client.connect("localhost", 8999);
    
    auto monster = host.spawnMonster(MonsterType::ZOMBIE, glm::vec2(0, 0));
    int initialLife = monster->getCurrentLife();
    
    // Client attacks monster
    client.sendAttack(monster->getId(), 20); // 20 damage
    
    host.receiveUpdate();
    EXPECT_EQ(monster->getCurrentLife(), initialLife - 20);
}

// STEP 3: Write test for multi-player state synchronization
TEST_F(NetworkGameTest, SynchronizeMultipleClients) {
    NetworkGame host;
    NetworkGame client1;
    NetworkGame client2;
    
    // Setup connections
    host.startHost(8999);
    client1.connect("localhost", 8999);
    client2.connect("localhost", 8999);
    
    // Host spawns a monster
    auto monster = host.spawnMonster(MonsterType::SKELETON, glm::vec2(100, 100));
    host.broadcastState();
    
    // Both clients should receive the monster state
    client1.receiveState();
    client2.receiveState();
    
    auto client1Monster = client1.getMonster(monster->getId());
    auto client2Monster = client2.getMonster(monster->getId());
    
    ASSERT_NE(client1Monster, nullptr);
    ASSERT_NE(client2Monster, nullptr);
    
    EXPECT_EQ(client1Monster->getType(), MonsterType::SKELETON);
    EXPECT_EQ(client2Monster->getType(), MonsterType::SKELETON);
    EXPECT_FLOAT_EQ(client1Monster->getPosition().x, 100.0f);
    EXPECT_FLOAT_EQ(client2Monster->getPosition().y, 100.0f);
}