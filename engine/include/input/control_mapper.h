#pragma once
#include <glm/glm.hpp>

namespace d2::input {

enum class ControlScheme {
    DIRECT,
    CURSOR
};

enum class ActionType {
    NONE,
    MOVE,
    ATTACK
};

enum class Direction {
    NORTH,
    NORTHEAST,
    EAST,
    SOUTHEAST,
    SOUTH,
    SOUTHWEST,
    WEST,
    NORTHWEST
};

enum class ButtonState {
    NONE = 0,
    ACTION_PRESSED = 1
};

struct AnalogInput {
    float x;
    float y;
};

struct Action {
    ActionType type = ActionType::NONE;
    Direction direction = Direction::NORTH;
};

class ControlMapper {
public:
    ControlMapper() : scheme_(ControlScheme::DIRECT) {}
    
    void setScheme(ControlScheme scheme);
    
    Action processInput(const AnalogInput& input, ButtonState buttons);

private:
    ControlScheme scheme_;
    
    Direction quantizeToD2Direction(float x, float y) const;
};

} // namespace d2::input