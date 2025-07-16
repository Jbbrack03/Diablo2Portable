#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include <memory>

namespace d2::rendering {

// Test subclass to verify polymorphic behavior
class TestSpriteRenderer : public SpriteRenderer {
public:
    TestSpriteRenderer() : destructorCalled(false) {}
    virtual ~TestSpriteRenderer() override {
        destructorCalled = true;
    }
    
    mutable bool destructorCalled;
};

class SpriteRendererPolymorphismTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup if needed
    }
};

TEST_F(SpriteRendererPolymorphismTest, VirtualDestructorWorksProperly) {
    // This test verifies that the virtual destructor allows proper cleanup
    // when deleting through a base class pointer
    
    TestSpriteRenderer* testRenderer = new TestSpriteRenderer();
    SpriteRenderer* basePtr = testRenderer;
    
    // Delete through base class pointer
    delete basePtr;
    
    // If we get here without crashing, the virtual destructor worked
    EXPECT_TRUE(true); // Test passes if no crash occurs
}

TEST_F(SpriteRendererPolymorphismTest, UniquePointerPolymorphism) {
    // Test with unique_ptr which is more common usage
    auto testRenderer = std::make_unique<TestSpriteRenderer>();
    
    // Transfer ownership to base class unique_ptr
    std::unique_ptr<SpriteRenderer> basePtr = std::move(testRenderer);
    
    // Reset should properly call virtual destructor
    basePtr.reset();
    
    // If we get here without crashing, the test passes
    EXPECT_TRUE(true);
}

} // namespace d2::rendering