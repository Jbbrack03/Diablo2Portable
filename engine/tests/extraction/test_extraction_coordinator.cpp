#include <gtest/gtest.h>
#include "extraction/extraction_coordinator.h"

namespace d2 {

class ExtractionCoordinatorTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

// Test that we can create an ExtractionCoordinator instance
TEST_F(ExtractionCoordinatorTest, CanCreateExtractionCoordinator) {
    ExtractionCoordinator coordinator;
    
    // If we can create it without errors, the test passes
    EXPECT_TRUE(true);
}

} // namespace d2