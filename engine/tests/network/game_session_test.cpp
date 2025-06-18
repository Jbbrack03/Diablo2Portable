#include <gtest/gtest.h>
#include "network/game_session.h"
#include "network/network_manager.h"

using namespace d2::network;

// Helper functions to create test data
PlayerInfo createTestPlayer(uint32_t id, const std::string& name, CharacterClass characterClass) {
    PlayerInfo player;
    player.id = id;
    player.name = name;
    player.characterClass = characterClass;
    return player;
}

GameStateMessage createTestMessage(MessageType type, uint32_t playerId, const glm::vec2& position) {
    GameStateMessage message;
    message.type = type;
    message.playerId = playerId;
    message.position = position;
    return message;
}

class GameSessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        networkManager = std::make_unique<NetworkManager>();
        networkManager->initialize();
        // Create a properly initialized session through NetworkManager
        *session = networkManager->hostGame("Test Game", 8);
    }
    
    std::unique_ptr<NetworkManager> networkManager;
    std::unique_ptr<GameSession> session = std::make_unique<GameSession>();
};

// Test initial state of properly initialized GameSession
TEST_F(GameSessionTest, InitialState) {
    EXPECT_TRUE(session->isActive());  // Session should be active after hostGame
    EXPECT_EQ(session->getPlayerCount(), 0);  // No players added yet
    EXPECT_EQ(session->getGameName(), "Test Game");  // Name set in hostGame
    EXPECT_EQ(session->getMaxPlayers(), 8);  // Max players set in hostGame
    EXPECT_EQ(session->getPort(), 6112);  // Default Diablo II port
}

// Test adding players to session
TEST_F(GameSessionTest, AddPlayer) {
    auto player = createTestPlayer(1, "TestPlayer", CharacterClass::BARBARIAN);
    
    EXPECT_TRUE(session->addPlayer(player));
    EXPECT_EQ(session->getPlayerCount(), 1);
}

// Test adding multiple players
TEST_F(GameSessionTest, AddMultiplePlayers) {
    auto player1 = createTestPlayer(1, "Player1", CharacterClass::BARBARIAN);
    auto player2 = createTestPlayer(2, "Player2", CharacterClass::SORCERESS);
    
    EXPECT_TRUE(session->addPlayer(player1));
    EXPECT_TRUE(session->addPlayer(player2));
    EXPECT_EQ(session->getPlayerCount(), 2);
}

// Test removing player from session
TEST_F(GameSessionTest, RemovePlayer) {
    auto player = createTestPlayer(1, "TestPlayer", CharacterClass::NECROMANCER);
    
    // Add player first
    EXPECT_TRUE(session->addPlayer(player));
    EXPECT_EQ(session->getPlayerCount(), 1);
    
    // Remove player
    EXPECT_TRUE(session->removePlayer(1));
    EXPECT_EQ(session->getPlayerCount(), 0);
}

// Test removing non-existent player
TEST_F(GameSessionTest, RemoveNonExistentPlayer) {
    EXPECT_FALSE(session->removePlayer(999));
    EXPECT_EQ(session->getPlayerCount(), 0);
}

// Test message sending when session is active
TEST_F(GameSessionTest, SendMessageWhenActive) {
    auto message = createTestMessage(MessageType::PLAYER_MOVE, 1, glm::vec2(100, 200));
    
    // Session is active after proper initialization
    EXPECT_TRUE(session->sendMessage(message));
}

// Test various message types
TEST_F(GameSessionTest, SendDifferentMessageTypes) {
    auto moveMessage = createTestMessage(MessageType::PLAYER_MOVE, 1, glm::vec2(100, 200));
    auto attackMessage = createTestMessage(MessageType::PLAYER_ATTACK, 2, glm::vec2(300, 400));
    auto chatMessage = createTestMessage(MessageType::CHAT_MESSAGE, 3, glm::vec2(0, 0));
    
    // All should succeed when session is active
    EXPECT_TRUE(session->sendMessage(moveMessage));
    EXPECT_TRUE(session->sendMessage(attackMessage));
    EXPECT_TRUE(session->sendMessage(chatMessage));
}

// Test player management with different character classes
TEST_F(GameSessionTest, AddPlayersWithDifferentClasses) {
    auto barbarian = createTestPlayer(1, "BarbarianPlayer", CharacterClass::BARBARIAN);
    auto amazon = createTestPlayer(2, "AmazonPlayer", CharacterClass::AMAZON);
    auto paladin = createTestPlayer(3, "PaladinPlayer", CharacterClass::PALADIN);
    auto assassin = createTestPlayer(4, "AssassinPlayer", CharacterClass::ASSASSIN);
    auto druid = createTestPlayer(5, "DruidPlayer", CharacterClass::DRUID);
    
    EXPECT_TRUE(session->addPlayer(barbarian));
    EXPECT_TRUE(session->addPlayer(amazon));
    EXPECT_TRUE(session->addPlayer(paladin));
    EXPECT_TRUE(session->addPlayer(assassin));
    EXPECT_TRUE(session->addPlayer(druid));
    
    EXPECT_EQ(session->getPlayerCount(), 5);
}

// Test removing specific player from multiple players
TEST_F(GameSessionTest, RemoveSpecificPlayerFromMultiple) {
    auto player1 = createTestPlayer(1, "Player1", CharacterClass::BARBARIAN);
    auto player2 = createTestPlayer(2, "Player2", CharacterClass::SORCERESS);
    auto player3 = createTestPlayer(3, "Player3", CharacterClass::NECROMANCER);
    
    session->addPlayer(player1);
    session->addPlayer(player2);
    session->addPlayer(player3);
    EXPECT_EQ(session->getPlayerCount(), 3);
    
    // Remove middle player
    EXPECT_TRUE(session->removePlayer(2));
    EXPECT_EQ(session->getPlayerCount(), 2);
    
    // Try to remove already removed player
    EXPECT_FALSE(session->removePlayer(2));
    EXPECT_EQ(session->getPlayerCount(), 2);
}

// Test maximum players limit
TEST_F(GameSessionTest, MaxPlayersLimit) {
    // Session is set up with maxPlayers = 8, try to add 9 players
    for (int i = 1; i <= 8; i++) {
        auto player = createTestPlayer(i, "Player" + std::to_string(i), CharacterClass::BARBARIAN);
        EXPECT_TRUE(session->addPlayer(player));
    }
    EXPECT_EQ(session->getPlayerCount(), 8);
    
    // 9th player should fail
    auto extraPlayer = createTestPlayer(9, "Player9", CharacterClass::SORCERESS);
    EXPECT_FALSE(session->addPlayer(extraPlayer));
    EXPECT_EQ(session->getPlayerCount(), 8);
}

// Test getters return correct values after initialization
TEST_F(GameSessionTest, GettersReturnCorrectValues) {
    // Test values after proper initialization through NetworkManager
    EXPECT_TRUE(session->isActive());
    EXPECT_EQ(session->getGameName(), "Test Game");
    EXPECT_EQ(session->getMaxPlayers(), 8);
    EXPECT_EQ(session->getPort(), 6112);
    EXPECT_EQ(session->getPlayerCount(), 0);
}