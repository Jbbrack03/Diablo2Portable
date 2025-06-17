#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/text_renderer.h"
#include "ui/font.h"

namespace d2 {

class TextRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        font_ = std::make_shared<Font>("Arial", 16);
    }
    
    std::shared_ptr<Font> font_;
};

TEST_F(TextRendererTest, CreateTextRenderer) {
    TextRenderer textRenderer;
    
    // Should be able to create a text renderer
    // Initially should have default color (white)
    EXPECT_EQ(textRenderer.getColor(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

} // namespace d2