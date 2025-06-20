#include <gtest/gtest.h>
#include "game/quest.h"
#include "game/quest_manager.h"

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

} // namespace d2