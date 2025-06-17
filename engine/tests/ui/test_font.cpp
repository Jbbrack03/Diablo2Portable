#include <gtest/gtest.h>
#include "ui/font.h"

namespace d2 {

class FontTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(FontTest, CreateBasicFont) {
    Font font("Arial", 16);
    
    // Font should have the properties set in constructor
    EXPECT_EQ(font.getName(), "Arial");
    EXPECT_EQ(font.getSize(), 16);
    
    // Default line height should be reasonable
    EXPECT_GT(font.getLineHeight(), 0);
}

} // namespace d2