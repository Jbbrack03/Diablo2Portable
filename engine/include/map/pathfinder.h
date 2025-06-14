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

private:
    // Helper method to smooth the path by removing unnecessary waypoints
    std::vector<PathNode> smoothPath(const std::vector<PathNode>& path, const Map& map);
    
    // Check if there's a clear line of sight between two points
    bool hasLineOfSight(int x1, int y1, int x2, int y2, const Map& map);
};

} // namespace d2::map