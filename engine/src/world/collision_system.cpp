#include "world/collision_system.h"
#include "world/collision_entity.h"
#include <algorithm>
#include <cmath>

bool CollisionSystem::checkAABBCollision(const AABB& box1, const AABB& box2) const {
    // Check if boxes overlap on both axes
    bool overlapX = box1.position.x + box1.size.x >= box2.position.x &&
                    box2.position.x + box2.size.x >= box1.position.x;
    
    bool overlapY = box1.position.y + box1.size.y >= box2.position.y &&
                    box2.position.y + box2.size.y >= box1.position.y;
    
    return overlapX && overlapY;
}

bool CollisionSystem::checkCircleCollision(const Circle& circle1, const Circle& circle2) const {
    glm::vec2 diff = circle2.center - circle1.center;
    float distanceSquared = diff.x * diff.x + diff.y * diff.y;
    float radiusSum = circle1.radius + circle2.radius;
    
    return distanceSquared <= radiusSum * radiusSum;
}

bool CollisionSystem::checkAABBCircleCollision(const AABB& box, const Circle& circle) const {
    // Find the closest point on the box to the circle center
    float closestX = std::max(box.position.x, std::min(circle.center.x, box.position.x + box.size.x));
    float closestY = std::max(box.position.y, std::min(circle.center.y, box.position.y + box.size.y));
    
    // Calculate distance from circle center to closest point
    glm::vec2 diff = circle.center - glm::vec2(closestX, closestY);
    float distanceSquared = diff.x * diff.x + diff.y * diff.y;
    
    return distanceSquared <= circle.radius * circle.radius;
}

void CollisionSystem::setWorldBounds(const glm::vec2& min, const glm::vec2& max) {
    minBounds = min;
    maxBounds = max;
}

bool CollisionSystem::isOutOfBounds(const AABB& entity) const {
    return entity.position.x < minBounds.x ||
           entity.position.y < minBounds.y ||
           entity.position.x + entity.size.x > maxBounds.x ||
           entity.position.y + entity.size.y > maxBounds.y;
}

void CollisionSystem::setCollisionMap(const std::vector<std::vector<int>>& map, float tileSz) {
    collisionMap = map;
    tileSize = tileSz;
}

bool CollisionSystem::checkMapCollision(const AABB& entity) const {
    if (collisionMap.empty()) {
        return false;
    }
    
    // Convert entity bounds to tile coordinates
    int startTileX = static_cast<int>(entity.position.x / tileSize);
    int startTileY = static_cast<int>(entity.position.y / tileSize);
    int endTileX = static_cast<int>((entity.position.x + entity.size.x) / tileSize);
    int endTileY = static_cast<int>((entity.position.y + entity.size.y) / tileSize);
    
    // Clamp to map bounds
    startTileX = std::max(0, startTileX);
    startTileY = std::max(0, startTileY);
    endTileX = std::min(static_cast<int>(collisionMap[0].size()) - 1, endTileX);
    endTileY = std::min(static_cast<int>(collisionMap.size()) - 1, endTileY);
    
    // Check if any tile in the entity's bounds is solid (1)
    for (int y = startTileY; y <= endTileY; ++y) {
        for (int x = startTileX; x <= endTileX; ++x) {
            if (collisionMap[y][x] == 1) {
                return true;  // Collision with wall
            }
        }
    }
    
    return false;
}

glm::vec2 CollisionSystem::getPenetrationVector(const AABB& box1, const AABB& box2) const {
    glm::vec2 penetration(0.0f, 0.0f);
    
    // Calculate overlap on each axis
    float overlapLeft = (box1.position.x + box1.size.x) - box2.position.x;
    float overlapRight = (box2.position.x + box2.size.x) - box1.position.x;
    float overlapTop = (box1.position.y + box1.size.y) - box2.position.y;
    float overlapBottom = (box2.position.y + box2.size.y) - box1.position.y;
    
    // Find minimum overlap
    float minOverlapX = std::min(overlapLeft, overlapRight);
    float minOverlapY = std::min(overlapTop, overlapBottom);
    
    // Resolve on the axis with smaller overlap
    if (minOverlapX < minOverlapY) {
        // Resolve horizontally
        if (overlapLeft < overlapRight) {
            penetration.x = -overlapLeft;  // Push box1 left
        } else {
            penetration.x = overlapRight;   // Push box1 right
        }
    } else {
        // Resolve vertically
        if (overlapTop < overlapBottom) {
            penetration.y = -overlapTop;    // Push box1 up
        } else {
            penetration.y = overlapBottom;   // Push box1 down
        }
    }
    
    return penetration;
}

void CollisionSystem::addEntity(std::shared_ptr<CollisionEntity> entity) {
    entities[entity->getId()] = entity;
    
    if (useSpatialGrid) {
        spatialGrid.addEntity(entity->getId(), entity->getPosition());
    }
}

void CollisionSystem::removeEntity(int entityId) {
    auto it = entities.find(entityId);
    if (it != entities.end()) {
        if (useSpatialGrid) {
            spatialGrid.removeEntity(entityId, it->second->getPosition());
        }
        entities.erase(it);
    }
}

void CollisionSystem::clear() {
    entities.clear();
    if (useSpatialGrid) {
        spatialGrid.cells.clear();
        useSpatialGrid = false;
    }
}

std::vector<std::pair<int, int>> CollisionSystem::checkCollisions() const {
    std::vector<std::pair<int, int>> collisions;
    
    // Check all entity pairs
    for (auto it1 = entities.begin(); it1 != entities.end(); ++it1) {
        auto it2 = it1;
        ++it2;
        
        for (; it2 != entities.end(); ++it2) {
            auto& entity1 = it1->second;
            auto& entity2 = it2->second;
            
            // Check layer collision rules
            if (!canLayersCollide(entity1->getLayer(), entity2->getLayer())) {
                continue;
            }
            
            bool collision = false;
            
            // Check collision based on shapes
            if (entity1->getShape() == CollisionShape::AABB && entity2->getShape() == CollisionShape::AABB) {
                AABB box1(entity1->getPosition(), entity1->getSize());
                AABB box2(entity2->getPosition(), entity2->getSize());
                collision = checkAABBCollision(box1, box2);
            } else if (entity1->getShape() == CollisionShape::CIRCLE && entity2->getShape() == CollisionShape::CIRCLE) {
                Circle circle1(entity1->getPosition(), entity1->getRadius());
                Circle circle2(entity2->getPosition(), entity2->getRadius());
                collision = checkCircleCollision(circle1, circle2);
            } else {
                // Mixed collision
                if (entity1->getShape() == CollisionShape::AABB) {
                    AABB box(entity1->getPosition(), entity1->getSize());
                    Circle circle(entity2->getPosition(), entity2->getRadius());
                    collision = checkAABBCircleCollision(box, circle);
                } else {
                    AABB box(entity2->getPosition(), entity2->getSize());
                    Circle circle(entity1->getPosition(), entity1->getRadius());
                    collision = checkAABBCircleCollision(box, circle);
                }
            }
            
            if (collision) {
                collisions.push_back({entity1->getId(), entity2->getId()});
            }
        }
    }
    
    return collisions;
}

std::vector<int> CollisionSystem::getCollidingEntities(int entityId) const {
    std::vector<int> collidingIds;
    
    auto it = entities.find(entityId);
    if (it == entities.end()) {
        return collidingIds;
    }
    
    auto& entity = it->second;
    
    for (const auto& [otherId, otherEntity] : entities) {
        if (otherId == entityId) {
            continue;
        }
        
        // Check layer collision rules
        if (!canLayersCollide(entity->getLayer(), otherEntity->getLayer())) {
            continue;
        }
        
        bool collision = false;
        
        // Check collision based on shapes
        if (entity->getShape() == CollisionShape::AABB && otherEntity->getShape() == CollisionShape::AABB) {
            AABB box1(entity->getPosition(), entity->getSize());
            AABB box2(otherEntity->getPosition(), otherEntity->getSize());
            collision = checkAABBCollision(box1, box2);
        } else if (entity->getShape() == CollisionShape::CIRCLE && otherEntity->getShape() == CollisionShape::CIRCLE) {
            Circle circle1(entity->getPosition(), entity->getRadius());
            Circle circle2(otherEntity->getPosition(), otherEntity->getRadius());
            collision = checkCircleCollision(circle1, circle2);
        } else {
            // Mixed collision
            if (entity->getShape() == CollisionShape::AABB) {
                AABB box(entity->getPosition(), entity->getSize());
                Circle circle(otherEntity->getPosition(), otherEntity->getRadius());
                collision = checkAABBCircleCollision(box, circle);
            } else {
                AABB box(otherEntity->getPosition(), otherEntity->getSize());
                Circle circle(entity->getPosition(), entity->getRadius());
                collision = checkAABBCircleCollision(box, circle);
            }
        }
        
        if (collision) {
            collidingIds.push_back(otherId);
        }
    }
    
    return collidingIds;
}

std::vector<int> CollisionSystem::getNearbyEntities(int entityId, float radius) const {
    std::vector<int> nearbyIds;
    
    auto it = entities.find(entityId);
    if (it == entities.end()) {
        return nearbyIds;
    }
    
    glm::vec2 position = it->second->getPosition();
    
    if (useSpatialGrid) {
        return spatialGrid.getNearbyEntities(position, radius);
    }
    
    // Fallback to brute force
    float radiusSquared = radius * radius;
    
    for (const auto& [otherId, otherEntity] : entities) {
        if (otherId == entityId) {
            continue;
        }
        
        glm::vec2 diff = otherEntity->getPosition() - position;
        float distSquared = diff.x * diff.x + diff.y * diff.y;
        
        if (distSquared <= radiusSquared) {
            nearbyIds.push_back(otherId);
        }
    }
    
    return nearbyIds;
}

void CollisionSystem::initializeSpatialGrid(float worldWidth, float worldHeight, float cellSize) {
    spatialGrid.initialize(worldWidth, worldHeight, cellSize);
    useSpatialGrid = true;
    
    // Add existing entities to grid
    for (const auto& [id, entity] : entities) {
        spatialGrid.addEntity(id, entity->getPosition());
    }
}

void CollisionSystem::setLayerCollision(int layer1, int layer2, bool canCollide) {
    layerCollisionMatrix[layer1][layer2] = canCollide;
    layerCollisionMatrix[layer2][layer1] = canCollide;  // Symmetric
}

bool CollisionSystem::canLayersCollide(int layer1, int layer2) const {
    auto it1 = layerCollisionMatrix.find(layer1);
    if (it1 != layerCollisionMatrix.end()) {
        auto it2 = it1->second.find(layer2);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    
    // Default: all layers can collide unless specified otherwise
    return true;
}

// SpatialGrid implementation
void CollisionSystem::SpatialGrid::initialize(float worldWidth, float worldHeight, float cellSz) {
    cellSize = cellSz;
    gridWidth = static_cast<int>(std::ceil(worldWidth / cellSize));
    gridHeight = static_cast<int>(std::ceil(worldHeight / cellSize));
    
    cells.clear();
    cells.resize(gridHeight);
    for (auto& row : cells) {
        row.resize(gridWidth);
    }
}

void CollisionSystem::SpatialGrid::addEntity(int entityId, const glm::vec2& position) {
    int cellX = static_cast<int>(position.x / cellSize);
    int cellY = static_cast<int>(position.y / cellSize);
    
    if (cellX >= 0 && cellX < gridWidth && cellY >= 0 && cellY < gridHeight) {
        cells[cellY][cellX].push_back(entityId);
    }
}

void CollisionSystem::SpatialGrid::removeEntity(int entityId, const glm::vec2& position) {
    int cellX = static_cast<int>(position.x / cellSize);
    int cellY = static_cast<int>(position.y / cellSize);
    
    if (cellX >= 0 && cellX < gridWidth && cellY >= 0 && cellY < gridHeight) {
        auto& cell = cells[cellY][cellX];
        cell.erase(std::remove(cell.begin(), cell.end(), entityId), cell.end());
    }
}

std::vector<int> CollisionSystem::SpatialGrid::getNearbyEntities(const glm::vec2& position, float radius) const {
    std::vector<int> nearbyEntities;
    
    // Calculate cell range to check
    int minCellX = static_cast<int>((position.x - radius) / cellSize);
    int maxCellX = static_cast<int>((position.x + radius) / cellSize);
    int minCellY = static_cast<int>((position.y - radius) / cellSize);
    int maxCellY = static_cast<int>((position.y + radius) / cellSize);
    
    // Clamp to grid bounds
    minCellX = std::max(0, minCellX);
    maxCellX = std::min(gridWidth - 1, maxCellX);
    minCellY = std::max(0, minCellY);
    maxCellY = std::min(gridHeight - 1, maxCellY);
    
    // Collect entities from relevant cells
    for (int y = minCellY; y <= maxCellY; ++y) {
        for (int x = minCellX; x <= maxCellX; ++x) {
            const auto& cell = cells[y][x];
            nearbyEntities.insert(nearbyEntities.end(), cell.begin(), cell.end());
        }
    }
    
    return nearbyEntities;
}