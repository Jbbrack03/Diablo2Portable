#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "save/save_manager.h"
#include "game/character.h"
#include "game/item.h"
#include "game/inventory.h"

namespace d2::save {

class SaveManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary directory for test saves
        m_testSaveDir = std::filesystem::temp_directory_path() / "d2_test_saves";
        std::filesystem::create_directories(m_testSaveDir);
    }
    
    void TearDown() override {
        // Clean up test directory
        std::filesystem::remove_all(m_testSaveDir);
    }
    
    std::filesystem::path m_testSaveDir;
};

// Test 1: SaveManager basic initialization
TEST_F(SaveManagerTest, BasicInitialization) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Verify save directory is set correctly
    EXPECT_EQ(saveManager.getSaveDirectory(), m_testSaveDir.string());
    
    // Verify save directory exists
    EXPECT_TRUE(std::filesystem::exists(m_testSaveDir));
}

// Test 2: Save character data to D2S format
TEST_F(SaveManagerTest, SaveCharacterToD2S) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Create a test character
    d2::game::Character testChar(d2::game::CharacterClass::SORCERESS);
    testChar.setLevel(42);
    
    // Save the character
    std::string saveFileName = "TestSorc.d2s";
    EXPECT_TRUE(saveManager.saveCharacter(testChar, saveFileName));
    
    // Verify the save file exists
    auto savePath = m_testSaveDir / saveFileName;
    EXPECT_TRUE(std::filesystem::exists(savePath));
    
    // Verify basic D2S file structure
    std::ifstream file(savePath, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    
    // D2S files start with a 4-byte signature (0xAA55AA55)
    uint32_t signature;
    file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
    EXPECT_EQ(signature, 0xAA55AA55);
}

// Test 3: Load character data from D2S format
TEST_F(SaveManagerTest, LoadCharacterFromD2S) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // First save a character
    d2::game::Character originalChar(d2::game::CharacterClass::BARBARIAN);
    originalChar.setLevel(25);
    
    std::string saveFileName = "TestBarb.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(originalChar, saveFileName));
    
    // Now load it back
    auto loadedChar = saveManager.loadCharacter(saveFileName);
    ASSERT_NE(loadedChar, nullptr);
    
    // Verify loaded character data matches
    EXPECT_EQ(loadedChar->getCharacterClass(), d2::game::CharacterClass::BARBARIAN);
    EXPECT_EQ(loadedChar->getLevel(), 25);
}

// Test 4: Checksum validation on load
TEST_F(SaveManagerTest, ChecksumValidation) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Create and save a character
    d2::game::Character testChar(d2::game::CharacterClass::NECROMANCER);
    testChar.setLevel(30);
    
    std::string saveFileName = "TestNecro.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(testChar, saveFileName));
    
    // Corrupt the save file by modifying a byte
    auto savePath = m_testSaveDir / saveFileName;
    std::fstream file(savePath, std::ios::binary | std::ios::in | std::ios::out);
    ASSERT_TRUE(file.is_open());
    
    // Change the level byte (offset 43) to corrupt the checksum
    file.seekp(43);
    uint8_t corruptLevel = 99;
    file.write(reinterpret_cast<char*>(&corruptLevel), 1);
    file.close();
    
    // Loading should fail due to invalid checksum
    auto loadedChar = saveManager.loadCharacter(saveFileName);
    EXPECT_EQ(loadedChar, nullptr);
}

// Test 5: Save inventory items to D2S format
TEST_F(SaveManagerTest, SaveInventoryItems) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Create a character with inventory items
    d2::game::Character testChar(d2::game::CharacterClass::PALADIN);
    testChar.setLevel(15);
    
    // Create a character inventory
    d2::game::Inventory inventory(10, 4);  // Standard D2 inventory size
    
    // Add some items to inventory
    auto sword = std::make_shared<d2::game::Item>("Short Sword", d2::game::ItemType::WEAPON);
    sword->setRarity(d2::game::ItemRarity::MAGIC);
    sword->setDamage(10, 20);
    
    auto armor = std::make_shared<d2::game::Item>("Leather Armor", d2::game::ItemType::ARMOR);
    armor->setRarity(d2::game::ItemRarity::RARE);
    armor->setDefense(50);
    
    // Add items to inventory
    inventory.addItem(sword, 0, 0);
    inventory.addItem(armor, 2, 0);
    
    // Save the character with inventory
    std::string saveFileName = "TestPaladin.d2s";
    ASSERT_TRUE(saveManager.saveCharacterWithInventory(testChar, inventory, saveFileName));
    
    // Verify the save file contains item data after the header
    auto savePath = m_testSaveDir / saveFileName;
    std::ifstream file(savePath, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    
    // Skip header
    file.seekg(765);
    
    // Read item list header (should have "JM" marker)
    char itemListMarker[2];
    file.read(itemListMarker, 2);
    EXPECT_EQ(itemListMarker[0], 'J');
    EXPECT_EQ(itemListMarker[1], 'M');
    
    // Read item count
    uint16_t itemCount;
    file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
    EXPECT_EQ(itemCount, 2);  // We added 2 items
}

// Test 6: Load inventory items from D2S format
TEST_F(SaveManagerTest, LoadInventoryItems) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // First save a character with inventory
    d2::game::Character originalChar(d2::game::CharacterClass::AMAZON);
    originalChar.setLevel(20);
    
    d2::game::Inventory originalInventory(10, 4);
    
    // Add a bow and some armor
    auto bow = std::make_shared<d2::game::Item>("Hunter's Bow", d2::game::ItemType::WEAPON);
    bow->setRarity(d2::game::ItemRarity::MAGIC);
    bow->setDamage(15, 25);
    bow->setSize(2, 3);  // Bows are typically 2x3
    
    auto helm = std::make_shared<d2::game::Item>("Cap", d2::game::ItemType::ARMOR);
    helm->setRarity(d2::game::ItemRarity::NORMAL);
    helm->setDefense(8);
    helm->setSize(2, 2);  // Helms are typically 2x2
    
    originalInventory.addItem(bow, 0, 0);
    originalInventory.addItem(helm, 3, 0);
    
    std::string saveFileName = "TestAmazon.d2s";
    ASSERT_TRUE(saveManager.saveCharacterWithInventory(originalChar, originalInventory, saveFileName));
    
    // Now load it back
    auto result = saveManager.loadCharacterWithInventory(saveFileName);
    ASSERT_NE(result.character, nullptr);
    ASSERT_NE(result.inventory, nullptr);
    
    // Verify character data
    EXPECT_EQ(result.character->getCharacterClass(), d2::game::CharacterClass::AMAZON);
    EXPECT_EQ(result.character->getLevel(), 20);
    
    // Verify inventory has the correct items
    EXPECT_FALSE(result.inventory->isEmpty());
    EXPECT_EQ(result.inventory->getUsedSlots(), 10);  // 2x3 + 2x2 = 10 slots
    
    // Check bow at position (0, 0)
    auto loadedBow = result.inventory->getItemAt(0, 0);
    ASSERT_NE(loadedBow, nullptr);
    EXPECT_EQ(loadedBow->getName(), "Hunter's Bow");
    EXPECT_EQ(loadedBow->getRarity(), d2::game::ItemRarity::MAGIC);
    EXPECT_EQ(loadedBow->getMinDamage(), 15);
    EXPECT_EQ(loadedBow->getMaxDamage(), 25);
    
    // Check helm at position (3, 0)
    auto loadedHelm = result.inventory->getItemAt(3, 0);
    ASSERT_NE(loadedHelm, nullptr);
    EXPECT_EQ(loadedHelm->getName(), "Cap");
    EXPECT_EQ(loadedHelm->getRarity(), d2::game::ItemRarity::NORMAL);
    EXPECT_EQ(loadedHelm->getDefense(), 8);
}

// Test 7: Save file backup system
TEST_F(SaveManagerTest, SaveFileBackup) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Create and save a character
    d2::game::Character testChar(d2::game::CharacterClass::DRUID);
    testChar.setLevel(35);
    
    std::string saveFileName = "TestDruid.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(testChar, saveFileName));
    
    // Modify and save again - should create backup
    testChar.setLevel(36);
    ASSERT_TRUE(saveManager.saveCharacter(testChar, saveFileName));
    
    // Check that backup was created
    auto backupPath = m_testSaveDir / "backup" / "TestDruid.d2s.bak";
    EXPECT_TRUE(std::filesystem::exists(backupPath));
    
    // Load the backup and verify it has the original data
    auto backupChar = saveManager.loadCharacterFromBackup(saveFileName);
    ASSERT_NE(backupChar, nullptr);
    EXPECT_EQ(backupChar->getLevel(), 35);  // Original level
    
    // Current save should have new data
    auto currentChar = saveManager.loadCharacter(saveFileName);
    ASSERT_NE(currentChar, nullptr);
    EXPECT_EQ(currentChar->getLevel(), 36);  // New level
}

// Test 8: Character stats persistence
TEST_F(SaveManagerTest, CharacterStatsPersistence) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Create a character with specific stats
    d2::game::Character testChar(d2::game::CharacterClass::ASSASSIN);
    testChar.setLevel(40);
    
    // Allocate stat points
    testChar.addStatPoint(d2::game::StatType::STRENGTH, 25);
    testChar.addStatPoint(d2::game::StatType::DEXTERITY, 30);
    testChar.addStatPoint(d2::game::StatType::VITALITY, 20);
    testChar.addStatPoint(d2::game::StatType::ENERGY, 15);
    
    // Save the character
    std::string saveFileName = "TestAssassin.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(testChar, saveFileName));
    
    // Load it back
    auto loadedChar = saveManager.loadCharacter(saveFileName);
    ASSERT_NE(loadedChar, nullptr);
    
    // Verify all stats were persisted correctly
    EXPECT_EQ(loadedChar->getLevel(), 40);
    EXPECT_EQ(loadedChar->getStrength(), 25 + 15);  // allocated + base assassin strength (15)
    EXPECT_EQ(loadedChar->getDexterity(), 30 + 15);  // allocated + base assassin dexterity (15)
    EXPECT_EQ(loadedChar->getVitality(), 20 + 15);  // allocated + base assassin vitality (15)
    EXPECT_EQ(loadedChar->getEnergy(), 15 + 15);  // allocated + base assassin energy (15)
}

// Test 9: Quest progress persistence
TEST_F(SaveManagerTest, QuestProgressPersistence) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Create a character with quest progress
    d2::game::Character testChar(d2::game::CharacterClass::AMAZON);
    testChar.setLevel(18);
    
    // Set some quest progress (Act 1 quests)
    testChar.setQuestComplete(0, true);  // Den of Evil complete
    testChar.setQuestComplete(1, true);  // Sisters' Burial Grounds complete
    testChar.setQuestComplete(2, false); // Search for Cain not complete
    testChar.setQuestComplete(3, true);  // The Forgotten Tower complete
    testChar.setQuestComplete(4, false); // Tools of the Trade not complete
    testChar.setQuestComplete(5, false); // Sisters to the Slaughter not complete
    
    // Save the character
    std::string saveFileName = "TestAmazonQuests.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(testChar, saveFileName));
    
    // Load it back
    auto loadedChar = saveManager.loadCharacter(saveFileName);
    ASSERT_NE(loadedChar, nullptr);
    
    // Verify quest progress was persisted
    EXPECT_TRUE(loadedChar->isQuestComplete(0));   // Den of Evil
    EXPECT_TRUE(loadedChar->isQuestComplete(1));   // Sisters' Burial Grounds
    EXPECT_FALSE(loadedChar->isQuestComplete(2));  // Search for Cain
    EXPECT_TRUE(loadedChar->isQuestComplete(3));   // The Forgotten Tower
    EXPECT_FALSE(loadedChar->isQuestComplete(4));  // Tools of the Trade
    EXPECT_FALSE(loadedChar->isQuestComplete(5));  // Sisters to the Slaughter
}

// Test 10: Waypoint activation persistence
TEST_F(SaveManagerTest, WaypointPersistence) {
    SaveManager saveManager(m_testSaveDir.string());
    
    // Create a character with waypoint progress
    d2::game::Character testChar(d2::game::CharacterClass::SORCERESS);
    testChar.setLevel(24);
    
    // Activate some waypoints
    // Act 1 waypoints
    testChar.activateWaypoint(0);   // Rogue Encampment (always active)
    testChar.activateWaypoint(1);   // Cold Plains
    testChar.activateWaypoint(3);   // Stony Field
    testChar.activateWaypoint(4);   // Dark Wood
    testChar.activateWaypoint(5);   // Black Marsh
    testChar.activateWaypoint(8);   // Catacombs Level 2
    
    // Act 2 waypoints
    testChar.activateWaypoint(9);   // Lut Gholein
    testChar.activateWaypoint(10);  // Sewers Level 2
    testChar.activateWaypoint(12);  // Halls of the Dead Level 2
    
    // Save the character
    std::string saveFileName = "TestSorcWaypoints.d2s";
    ASSERT_TRUE(saveManager.saveCharacter(testChar, saveFileName));
    
    // Load it back
    auto loadedChar = saveManager.loadCharacter(saveFileName);
    ASSERT_NE(loadedChar, nullptr);
    
    // Verify waypoint activation was persisted
    EXPECT_TRUE(loadedChar->isWaypointActive(0));   // Rogue Encampment
    EXPECT_TRUE(loadedChar->isWaypointActive(1));   // Cold Plains
    EXPECT_FALSE(loadedChar->isWaypointActive(2));  // Underground Passage (not activated)
    EXPECT_TRUE(loadedChar->isWaypointActive(3));   // Stony Field
    EXPECT_TRUE(loadedChar->isWaypointActive(4));   // Dark Wood
    EXPECT_TRUE(loadedChar->isWaypointActive(5));   // Black Marsh
    EXPECT_FALSE(loadedChar->isWaypointActive(6));  // Tamoe Highland (not activated)
    EXPECT_FALSE(loadedChar->isWaypointActive(7));  // Monastery Gate (not activated)
    EXPECT_TRUE(loadedChar->isWaypointActive(8));   // Catacombs Level 2
    EXPECT_TRUE(loadedChar->isWaypointActive(9));   // Lut Gholein
    EXPECT_TRUE(loadedChar->isWaypointActive(10));  // Sewers Level 2
    EXPECT_FALSE(loadedChar->isWaypointActive(11)); // Dry Hills (not activated)
    EXPECT_TRUE(loadedChar->isWaypointActive(12));  // Halls of the Dead Level 2
}

} // namespace d2::save