#include <gtest/gtest.h>
#include "game/quest.h"
#include "game/quest_manager.h"
#include "game/monster.h"
#include "game/game_engine.h"

namespace d2 {

class QuestSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        questManager = std::make_unique<QuestManager>();
    }

    std::unique_ptr<QuestManager> questManager;
};

// Test 1: Basic quest creation
TEST_F(QuestSystemTest, CreateBasicQuest) {
    // Create a simple kill quest
    auto quest = questManager->createQuest(
        QuestId::DEN_OF_EVIL,
        "Den of Evil", 
        "Clear the Den of Evil",
        QuestType::KILL_MONSTERS
    );
    
    EXPECT_NE(quest, nullptr);
    EXPECT_EQ(quest->getId(), QuestId::DEN_OF_EVIL);
    EXPECT_EQ(quest->getName(), "Den of Evil");
    EXPECT_EQ(quest->getDescription(), "Clear the Den of Evil");
    EXPECT_EQ(quest->getType(), QuestType::KILL_MONSTERS);
    EXPECT_FALSE(quest->isComplete());
    EXPECT_TRUE(quest->isActive());
}

// Test 2: Quest objectives tracking
TEST_F(QuestSystemTest, TrackKillObjectives) {
    auto quest = questManager->createQuest(
        QuestId::DEN_OF_EVIL,
        "Den of Evil", 
        "Clear the Den of Evil",
        QuestType::KILL_MONSTERS
    );
    
    // Set kill requirement
    quest->setKillRequirement(game::MonsterType::FALLEN, 5);
    
    EXPECT_EQ(quest->getRequiredKills(game::MonsterType::FALLEN), 5);
    EXPECT_EQ(quest->getCurrentKills(game::MonsterType::FALLEN), 0);
    EXPECT_FALSE(quest->isComplete());
    
    // Track kills
    quest->recordKill(game::MonsterType::FALLEN);
    quest->recordKill(game::MonsterType::FALLEN);
    
    EXPECT_EQ(quest->getCurrentKills(game::MonsterType::FALLEN), 2);
    EXPECT_FALSE(quest->isComplete());
    
    // Complete the requirement
    quest->recordKill(game::MonsterType::FALLEN);
    quest->recordKill(game::MonsterType::FALLEN);
    quest->recordKill(game::MonsterType::FALLEN);
    
    EXPECT_EQ(quest->getCurrentKills(game::MonsterType::FALLEN), 5);
    EXPECT_TRUE(quest->isComplete());
}

// Test 3: Quest system integration with GameEngine
TEST_F(QuestSystemTest, IntegrateWithGameEngine) {
    // Create a quest
    auto quest = questManager->createQuest(
        QuestId::DEN_OF_EVIL,
        "Den of Evil", 
        "Clear the Den of Evil",
        QuestType::KILL_MONSTERS
    );
    quest->setKillRequirement(game::MonsterType::FALLEN, 3);
    
    // Integrate quest manager with game events
    questManager->startTracking(quest);
    
    // Simulate monster kills
    questManager->onMonsterKilled(game::MonsterType::FALLEN);
    questManager->onMonsterKilled(game::MonsterType::FALLEN);
    
    EXPECT_FALSE(quest->isComplete());
    
    questManager->onMonsterKilled(game::MonsterType::FALLEN);
    
    EXPECT_TRUE(quest->isComplete());
    EXPECT_TRUE(questManager->isQuestComplete(QuestId::DEN_OF_EVIL));
}

// Test 4: GameEngine quest integration
TEST_F(QuestSystemTest, GameEngineQuestTracking) {
    // This test will verify that GameEngine has QuestManager
    // For now, we'll just test that we can get the quest manager from game engine
    GameEngine gameEngine;
    gameEngine.initialize();
    
    auto* engineQuestManager = gameEngine.getQuestManager();
    EXPECT_NE(engineQuestManager, nullptr);
    
    // Create a quest through the engine's quest manager
    auto quest = engineQuestManager->createQuest(
        QuestId::BLOOD_RAVEN,
        "Blood Raven",
        "Defeat Blood Raven",
        QuestType::KILL_BOSS
    );
    
    EXPECT_NE(quest, nullptr);
    EXPECT_EQ(quest->getName(), "Blood Raven");
}

} // namespace d2