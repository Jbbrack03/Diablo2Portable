#pragma once

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <glm/vec2.hpp>
#include "rendering/vertex_buffer.h"

namespace d2 {
    class TextureAtlas;
}

namespace d2::rendering {

class Renderer;
class TextureManager;
class ShaderManager;
class VertexBuffer;
class VertexArrayObject;

class SpriteRenderer {
public:
    SpriteRenderer(); // Custom constructor needed for unique_ptr with forward declaration
    virtual ~SpriteRenderer(); // Virtual destructor for proper polymorphic deletion

    bool initialize(const Renderer& renderer, const TextureManager& texture_manager);
    virtual void beginFrame();
    virtual void drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size);
    virtual void drawSpriteFromAtlas(const std::string& spriteName, const glm::vec2& position, const glm::vec2& size);
    virtual void endFrame();
    
    // Batch rendering for performance optimization
    void beginBatch();
    void endBatch();
    
    // Texture Atlas support
    void addAtlas(const d2::TextureAtlas& atlas);
    uint32_t getAtlasCount() const;
    
    // Alpha blending support
    void enableAlphaBlending();
    void disableAlphaBlending();
    
    // Depth testing support
    void enableDepthTesting();
    void disableDepthTesting();
    void enableDepthWrites();
    void disableDepthWrites();
    
    // Alpha testing support
    void enableAlphaTesting(float threshold);
    void disableAlphaTesting();
    bool isAlphaTestingEnabled() const;
    float getAlphaTestThreshold() const;
    std::string getFragmentShaderSource() const;
    
    uint32_t getDrawCallCount() const;
    uint32_t getSpriteCount() const;
    
    // Shader management
    uint32_t getShaderProgram() const;
    bool isShaderProgramActive() const;
    
    // OpenGL resource access (for testing)
    uint32_t getVAOId() const;
    uint32_t getVertexBufferId() const;

private:
    bool initialized_ = false;
    uint32_t draw_call_count_ = 0;
    uint32_t sprite_count_ = 0;
    std::unordered_set<uint32_t> textures_used_;
    
    // Alpha testing state
    bool alpha_testing_enabled_ = false;
    float alpha_test_threshold_ = 0.5f;
    
    // Shader management
    std::unique_ptr<ShaderManager> shader_manager_;
    uint32_t shader_program_ = 0;
    bool shader_program_active_ = false;
    
    // OpenGL resources
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<VertexArrayObject> vao_;
    
    // Sprite batching
    struct SpriteBatch {
        uint32_t texture_id;
        std::vector<SpriteVertex> vertices;
    };
    std::unordered_map<uint32_t, SpriteBatch> sprite_batches_;
    
    // Texture Atlas support
    std::vector<d2::TextureAtlas> atlases_;
};

} // namespace d2::rendering