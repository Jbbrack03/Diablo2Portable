#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "input/control_mapper.h"
#include <glm/glm.hpp>

using namespace d2::input;

class ControlMapperTest : public ::testing::Test {
protected:
    void SetUp() override {
        mapper = std::make_unique<ControlMapper>();
    }

    std::unique_ptr<ControlMapper> mapper;
};

TEST_F(ControlMapperTest, DirectMovement) {
    mapper->setScheme(ControlScheme::DIRECT);
    
    // Test northeast movement (diagonal)
    AnalogInput input{0.7f, 0.7f};
    ButtonState buttons = ButtonState::NONE;
    
    auto action = mapper->processInput(input, buttons);
    
    EXPECT_EQ(action.type, ActionType::MOVE);
    EXPECT_EQ(action.direction, Direction::NORTHEAST);
}