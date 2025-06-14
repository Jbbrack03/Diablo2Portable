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
    
    // Check if using octile distance heuristic
    bool isUsingOctileDistance() const { return m_useOctileDistance; }
    
    // Get number of nodes explored in last pathfinding operation
    int getNodesExplored() const { return m_nodesExplored; }

private:
    // Helper method to smooth the path by removing unnecessary waypoints
    std::vector<PathNode> smoothPath(const std::vector<PathNode>& path, const Map& map);
    
    // Check if there's a clear line of sight between two points
    bool hasLineOfSight(int x1, int y1, int x2, int y2, const Map& map);
    
    bool m_useOctileDistance = true;  // Using octile distance for grid-based movement
    mutable int m_nodesExplored = 0;
};

} // namespace d2::map