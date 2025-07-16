#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include "game/item_database.h"
#include "utils/data_table_parser.h"
#include "core/asset_manager.h"

class ItemDatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        assetManager_ = std::make_shared<d2portable::core::AssetManager>();
        assetManager_->initialize("test_assets");
        
        parser_ = std::make_unique<d2::utils::DataTableParser>();
        itemDb_ = std::make_unique<d2::game::ItemDatabase>();
    }
    
    std::shared_ptr<d2portable::core::AssetManager> assetManager_;
    std::unique_ptr<d2::utils::DataTableParser> parser_;
    std::unique_ptr<d2::game::ItemDatabase> itemDb_;
};

TEST_F(ItemDatabaseTest, LoadsFromDataTables) {
    // Create test armor data file
    std::string armorData = R"(name	invfile	type	ac	minac	maxac	reqstr	durability	level	rarity	cost	invwidth	invheight
Quilted Armor	inv_armor_quilted	body	8	8	11	12	20	1	1	200	2	3
Leather Armor	inv_armor_leather	body	14	14	17	15	24	2	1	350	2	3)";
    
    // Save to test file
    std::ofstream file("test_assets/data/armor.txt");
    file << armorData;
    file.close();
    
    // Load data tables from asset manager
    itemDb_->loadFromAssetManager(assetManager_, parser_.get());
    
    // Check if items were loaded
    EXPECT_TRUE(itemDb_->hasItem("Quilted Armor"));
    EXPECT_TRUE(itemDb_->hasItem("Leather Armor"));
    
    auto quilted = itemDb_->getItem("Quilted Armor");
    EXPECT_EQ(quilted.getDefense(), 8);
    EXPECT_EQ(quilted.getRequiredStrength(), 12);
}

TEST_F(ItemDatabaseTest, LoadsWeaponData) {
    // Create test weapon data file
    std::string weaponData = R"(name	invfile	type	mindamage	maxdamage	reqstr	reqdex	durability	level	cost	invwidth	invheight	speed
Short Sword	inv_sword_short	sword	2	7	0	0	24	1	50	1	3	0
Long Sword	inv_sword_long	sword	3	10	25	0	32	5	120	1	3	0)";
    
    std::ofstream file("test_assets/data/weapons.txt");
    file << weaponData;
    file.close();
    
    itemDb_->loadFromAssetManager(assetManager_, parser_.get());
    
    auto shortSword = itemDb_->getItem("Short Sword");
    EXPECT_EQ(shortSword.getMinDamage(), 2);
    EXPECT_EQ(shortSword.getMaxDamage(), 7);
}

TEST_F(ItemDatabaseTest, LoadsMiscItemData) {
    // Create test misc item data file
    std::string miscData = R"(name	invfile	type	stackable	maxstack	cost	level
Health Potion	inv_potion_health	potion	1	1	50	1
Gold	inv_gold	gold	1	50000	1	1)";
    
    std::ofstream file("test_assets/data/misc.txt");
    file << miscData;
    file.close();
    
    itemDb_->loadFromAssetManager(assetManager_, parser_.get());
    
    auto potion = itemDb_->getItem("Health Potion");
    EXPECT_TRUE(potion.isStackable());
    EXPECT_EQ(potion.getMaxStackSize(), 1);
}

TEST_F(ItemDatabaseTest, CachesLoadedData) {
    // Create minimal test data
    std::string armorData = "name\tac\nTest Armor\t10";
    std::ofstream file("test_assets/data/armor.txt");
    file << armorData;
    file.close();
    
    // First load
    itemDb_->loadFromAssetManager(assetManager_, parser_.get());
    EXPECT_TRUE(itemDb_->hasItem("Test Armor"));
    
    // Create new test data with different value
    std::string newArmorData = "name\tac\nTest Armor\t20";
    std::ofstream file2("test_assets/data/armor.txt");
    file2 << newArmorData;
    file2.close();
    
    // Second load should not reload data (item already exists)
    itemDb_->loadFromAssetManager(assetManager_, parser_.get());
    
    // Should still have the original value (cached)
    auto item = itemDb_->getItem("Test Armor");
    EXPECT_EQ(item.getDefense(), 10);
}