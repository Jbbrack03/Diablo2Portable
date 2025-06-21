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