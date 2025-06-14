#include "map/pathfinder.h"
#include "map/map_loader.h"
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace d2::map {

// Helper struct for A* algorithm
struct AStarNode {
    int x, y;
    float g_cost;  // Cost from start to this node
    float h_cost;  // Heuristic cost from this node to goal
    float f_cost;  // Total cost (g + h)
    AStarNode* parent;
    
    AStarNode(int px, int py, float g, float h, AStarNode* p = nullptr) 
        : x(px), y(py), g_cost(g), h_cost(h), f_cost(g + h), parent(p) {}
};

// Comparator for priority queue (min heap based on f_cost)
struct NodeComparator {
    bool operator()(const AStarNode* a, const AStarNode* b) const {
        return a->f_cost > b->f_cost;
    }
};

// Hash function for coordinates
struct CoordHash {
    std::size_t operator()(const std::pair<int, int>& coord) const {
        return std::hash<int>()(coord.first) ^ (std::hash<int>()(coord.second) << 1);
    }
};

std::vector<PathNode> Pathfinder::findPath(int startX, int startY, int goalX, int goalY, const Map& map) {
    // Debug output
    bool debug = false;
    if (debug) {
        std::cerr << "=== PATHFINDING DEBUG: (" << startX << "," << startY << ") to (" 
                  << goalX << "," << goalY << ") ===" << std::endl;
    }
    
    // Check if start and goal are walkable
    if (!map.isWalkable(startX, startY) || !map.isWalkable(goalX, goalY)) {
        return {};
    }
    
    // Check if start equals goal
    if (startX == goalX && startY == goalY) {
        return {PathNode(startX, startY)};
    }
    
    // Priority queue for open nodes
    std::priority_queue<AStarNode*, std::vector<AStarNode*>, NodeComparator> openSet;
    
    // Set to track closed nodes
    std::unordered_set<std::pair<int, int>, CoordHash> closedSet;
    
    // Map to track the best g_cost for each position
    std::unordered_map<std::pair<int, int>, float, CoordHash> bestCosts;
    
    // Store all allocated nodes for cleanup
    std::vector<std::unique_ptr<AStarNode>> allNodes;
    
    // Heuristic function (Euclidean distance for diagonal movement)
    auto heuristic = [](int x1, int y1, int x2, int y2) -> float {
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        return std::sqrt(static_cast<float>(dx * dx + dy * dy));
    };
    
    // Create start node
    auto startNode = std::make_unique<AStarNode>(startX, startY, 0.0f, 
                                                  heuristic(startX, startY, goalX, goalY));
    openSet.push(startNode.get());
    bestCosts[{startX, startY}] = 0.0f;
    allNodes.push_back(std::move(startNode));
    
    // Define neighbors (8 directions for diagonal movement)
    const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const float costs[] = {1.414f, 1.0f, 1.414f, 1.0f, 1.0f, 1.414f, 1.0f, 1.414f}; // Diagonal = sqrt(2)
    
    int iterations = 0;
    int nodesExplored = 0;
    const int maxIterations = 10000;  // Increase safety limit
    while (!openSet.empty() && iterations < maxIterations) {
        iterations++;
        
        // Get node with lowest f_cost
        AStarNode* current = openSet.top();
        openSet.pop();
        
        // Get current position
        auto currentPos = std::make_pair(current->x, current->y);
        
        // Skip if already in closed set
        if (closedSet.find(currentPos) != closedSet.end()) {
            continue;
        }
        
        nodesExplored++;
        
        
        // Check if we reached the goal
        if (current->x == goalX && current->y == goalY) {
            // Reconstruct path
            std::vector<PathNode> path;
            AStarNode* node = current;
            while (node != nullptr) {
                path.emplace_back(node->x, node->y);
                node = node->parent;
            }
            std::reverse(path.begin(), path.end());
            
            // Apply path smoothing
            return smoothPath(path, map);
        }
        
        // Mark as closed
        closedSet.insert(currentPos);
        
        
        // Check all neighbors
        for (int i = 0; i < 8; i++) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];
            
            // Check if neighbor is walkable
            if (!map.isWalkable(nx, ny)) {
                continue;
            }
            
            // Skip if already in closed set
            auto neighborPos = std::make_pair(nx, ny);
            if (closedSet.count(neighborPos) > 0) {
                continue;
            }
            
            // For diagonal movement, check if we can actually move diagonally
            // (both adjacent cells must be walkable)
            if (i == 0 || i == 2 || i == 5 || i == 7) {  // Diagonal moves
                bool canMoveDiagonally = true;
                if (i == 0) {  // NW
                    canMoveDiagonally = map.isWalkable(current->x - 1, current->y) && 
                                       map.isWalkable(current->x, current->y - 1);
                } else if (i == 2) {  // NE
                    canMoveDiagonally = map.isWalkable(current->x + 1, current->y) && 
                                       map.isWalkable(current->x, current->y - 1);
                } else if (i == 5) {  // SW
                    canMoveDiagonally = map.isWalkable(current->x - 1, current->y) && 
                                       map.isWalkable(current->x, current->y + 1);
                } else if (i == 7) {  // SE
                    canMoveDiagonally = map.isWalkable(current->x + 1, current->y) && 
                                       map.isWalkable(current->x, current->y + 1);
                }
                
                if (!canMoveDiagonally) {
                    continue;
                }
            }
            
            float tentative_g = current->g_cost + costs[i];
            
            // Check if we've found a better path to this neighbor
            auto it = bestCosts.find(neighborPos);
            if (it != bestCosts.end() && tentative_g >= it->second) {
                continue;
            }
            
            // Create new node
            float h = heuristic(nx, ny, goalX, goalY);
            auto newNode = std::make_unique<AStarNode>(nx, ny, tentative_g, h, current);
            openSet.push(newNode.get());
            bestCosts[neighborPos] = tentative_g;
            
            
            allNodes.push_back(std::move(newNode));
        }
    }
    
    // No path found
    if (iterations >= maxIterations) {
        // Hit iteration limit - this shouldn't happen for reasonable maps
        // For debugging: could log this condition
    }
    return {};
}

std::vector<PathNode> Pathfinder::smoothPath(const std::vector<PathNode>& path, const Map& map) {
    if (path.size() <= 2) {
        return path;  // Already optimal
    }
    
    std::vector<PathNode> smoothed;
    smoothed.push_back(path[0]);  // Always include start
    
    size_t current = 0;
    while (current < path.size() - 1) {
        size_t farthest = current + 1;
        
        // Find the farthest point we can reach with line of sight
        for (size_t i = path.size() - 1; i > current + 1; i--) {
            if (hasLineOfSight(path[current].x, path[current].y, 
                              path[i].x, path[i].y, map)) {
                farthest = i;
                break;
            }
        }
        
        smoothed.push_back(path[farthest]);
        current = farthest;
    }
    
    return smoothed;
}

bool Pathfinder::hasLineOfSight(int x1, int y1, int x2, int y2, const Map& map) {
    // Use Bresenham's line algorithm to check all cells along the line
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int x = x1;
    int y = y1;
    
    while (true) {
        // Check if current position is walkable
        if (!map.isWalkable(x, y)) {
            return false;
        }
        
        // Reached the target
        if (x == x2 && y == y2) {
            return true;
        }
        
        // Bresenham's algorithm step
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

} // namespace d2::map