#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "utils/data_table_parser.h"

class DataTableParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = std::make_unique<d2::utils::DataTableParser>();
    }
    
    std::unique_ptr<d2::utils::DataTableParser> parser_;
};

TEST_F(DataTableParserTest, ParsesExcelFiles) {
    // Test data simulating Diablo 2 armor.txt format
    std::string armorData = R"(name	invfile	type	ac	minac	maxac	reqstr	durability	level	rarity	cost	invwidth	invheight
Quilted Armor	inv_armor_quilted	body	8	8	11	12	20	1	1	200	2	3
Leather Armor	inv_armor_leather	body	14	14	17	15	24	2	1	350	2	3
Hard Leather Armor	inv_armor_hardleather	body	21	21	24	20	28	5	1	450	2	3
Studded Leather	inv_armor_studded	body	32	32	35	27	32	8	1	725	2	3
Ring Mail	inv_armor_ring	body	45	45	48	36	26	11	1	900	2	3)";
    
    auto table = parser_->parseExcel(armorData);
    
    EXPECT_GT(table.getRowCount(), 0);
    EXPECT_TRUE(table.hasColumn("name"));
    EXPECT_TRUE(table.hasColumn("ac"));
    
    auto quilted = table.findRow("name", "Quilted Armor");
    EXPECT_EQ(quilted["ac"], "8");
}

TEST_F(DataTableParserTest, HandlesEmptyData) {
    auto table = parser_->parseExcel("");
    EXPECT_EQ(table.getRowCount(), 0);
}

TEST_F(DataTableParserTest, HandlesTabDelimitedFormat) {
    std::string weaponData = "name\tdamage\tspeed\n";
    weaponData += "Short Sword\t2-7\tfast\n";
    weaponData += "Long Sword\t3-10\tnormal\n";
    
    auto table = parser_->parseExcel(weaponData);
    
    EXPECT_EQ(table.getRowCount(), 2);
    auto shortSword = table.findRow("name", "Short Sword");
    EXPECT_EQ(shortSword["damage"], "2-7");
    EXPECT_EQ(shortSword["speed"], "fast");
}

TEST_F(DataTableParserTest, ParsesNumericValues) {
    std::string miscData = "name\tvalue\tstack\n";
    miscData += "Gold Coin\t1\t50000\n";
    miscData += "Health Potion\t50\t1\n";
    
    auto table = parser_->parseExcel(miscData);
    
    auto gold = table.findRow("name", "Gold Coin");
    EXPECT_EQ(table.getIntValue(gold, "value"), 1);
    EXPECT_EQ(table.getIntValue(gold, "stack"), 50000);
}

TEST_F(DataTableParserTest, HandlesQuotedStrings) {
    std::string textData = "id\ttext\tdescription\n";
    textData += "1\t\"Hello, world!\"\t\"This is a test\"\n";
    textData += "2\t\"Multi\nline\"\t\"Has tabs\ttoo\"\n";
    
    auto table = parser_->parseExcel(textData);
    
    auto row1 = table.findRow("id", "1");
    EXPECT_EQ(row1["text"], "Hello, world!");
    EXPECT_EQ(row1["description"], "This is a test");
}