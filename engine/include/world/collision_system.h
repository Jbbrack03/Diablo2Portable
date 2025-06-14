#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <unordered_map>

// Basic collision shapes
struct AABB {
    glm::vec2 position;  // Top-left corner
    glm::vec2 size;      // Width and height
    
    AABB(const glm::vec2& pos = glm::vec2(0.0f), const glm::vec2& sz = glm::vec2(0.0f))
        : position(pos), size(sz) {}
};

struct Circle {
    glm::vec2 center;
    float radius;
    
    Circle(const glm::vec2& c = glm::vec2(0.0f), float r = 0.0f)
        : center(c), radius(r) {}
};

// Forward declarations
class CollisionEntity;

class CollisionSystem {
public:
    CollisionSystem() = default;
    ~CollisionSystem() = default;
    
    // Basic shape collision detection
    bool checkAABBCollision(const AABB& box1, const AABB& box2) const;
    bool checkCircleCollision(const Circle& circle1, const Circle& circle2) const;
    bool checkAABBCircleCollision(const AABB& box, const Circle& circle) const;
    
    // World bounds
    void setWorldBounds(const glm::vec2& minBounds, const glm::vec2& maxBounds);
    bool isOutOfBounds(const AABB& entity) const;
    
    // Tile-based collision map
    void setCollisionMap(const std::vector<std::vector<int>>& map, float tileSize);
    bool checkMapCollision(const AABB& entity) const;
    
    // Collision resolution helpers
    glm::vec2 getPenetrationVector(const AABB& box1, const AABB& box2) const;
    
    // Entity management
    void addEntity(std::shared_ptr<CollisionEntity> entity);
    void removeEntity(int entityId);
    void clear();
    size_t getEntityCount() const { return entities.size(); }
    
    // Entity collision detection
    std::vector<std::pair<int, int>> checkCollisions() const;
    std::vector<int> getCollidingEntities(int entityId) const;
    std::vector<int> getNearbyEntities(int entityId, float radius) const;
    
    // Spatial optimization
    void initializeSpatialGrid(float worldWidth, float worldHeight, float cellSize);
    
    // Layer collision settings
    void setLayerCollision(int layer1, int layer2, bool canCollide);
    
    // Get entities for collision response
    std::unordered_map<int, std::shared_ptr<CollisionEntity>>& getEntities() { return entities; }
    std::shared_ptr<CollisionEntity> getEntity(int id) {
        auto it = entities.find(id);
        return (it != entities.end()) ? it->second : nullptr;
    }
    
private:
    // World bounds
    glm::vec2 minBounds{0.0f, 0.0f};
    glm::vec2 maxBounds{1000.0f, 1000.0f};
    
    // Tile collision map
    std::vector<std::vector<int>> collisionMap;
    float tileSize = 32.0f;
    
    // Entities
    std::unordered_map<int, std::shared_ptr<CollisionEntity>> entities;
    
    // Spatial grid for optimization
    struct SpatialGrid {
        float cellSize = 100.0f;
        int gridWidth = 10;
        int gridHeight = 10;
        std::vector<std::vector<std::vector<int>>> cells;  // Grid of entity IDs
        
        void initialize(float worldWidth, float worldHeight, float cellSz);
        void addEntity(int entityId, const glm::vec2& position);
        void removeEntity(int entityId, const glm::vec2& position);
        std::vector<int> getNearbyEntities(const glm::vec2& position, float radius) const;
    };
    
    SpatialGrid spatialGrid;
    bool useSpatialGrid = false;
    
    // Layer collision matrix
    std::unordered_map<int, std::unordered_map<int, bool>> layerCollisionMatrix;
    
    // Helper functions
    bool canLayersCollide(int layer1, int layer2) const;
};