#pragma once

#include "rendering/world_renderer.h"
#include "performance/spatial_grid.h"
#include <memory>

namespace d2::game {
    class Monster;
}

namespace d2::rendering {

class OptimizedWorldRenderer : public WorldRenderer {
public:
    OptimizedWorldRenderer();
    ~OptimizedWorldRenderer() = default;
    
    void initialize(const d2portable::core::AssetManager& assetManager) override;
    void render(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer) override;
    void renderWithCamera(const d2::game::GameState& gameState, 
                         SpriteRenderer& spriteRenderer, 
                         const Camera& camera) override;
    
    // Enable/disable optimizations for testing
    void setOptimizationsEnabled(bool enabled) { optimizationsEnabled_ = enabled; }
    bool areOptimizationsEnabled() const { return optimizationsEnabled_; }
    
    // Performance statistics
    int getRenderedEntityCount() const { return lastRenderedEntityCount_; }
    int getCulledEntityCount() const { return lastCulledEntityCount_; }
    
private:
    bool optimizationsEnabled_ = true;
    int lastRenderedEntityCount_ = 0;
    int lastCulledEntityCount_ = 0;
    
    // Spatial grid for monsters
    std::unique_ptr<d2::performance::SpatialGrid<d2::game::Monster>> monsterGrid_;
    
    // Viewport bounds for culling
    glm::vec2 viewportMin_;
    glm::vec2 viewportMax_;
    
    void updateSpatialGrid(const d2::game::GameState& gameState);
    bool isInViewport(const glm::vec2& position, const glm::vec2& size) const;
};

} // namespace d2::rendering