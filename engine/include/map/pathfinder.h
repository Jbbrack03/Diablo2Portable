#pragma once

#include <vector>

namespace d2::map {

class Map;

struct PathNode {
    int x;
    int y;
    
    PathNode(int px, int py) : x(px), y(py) {}
};

class Pathfinder {
public:
    std::vector<PathNode> findPath(int startX, int startY, int goalX, int goalY, const Map& map);
};

} // namespace d2::map