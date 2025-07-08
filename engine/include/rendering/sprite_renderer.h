#pragma once

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include "rendering/vertex_buffer.h"

namespace d2::rendering {

class Renderer;
class TextureManager;
class ShaderManager;
class VertexBuffer;
class VertexArrayObject;

class SpriteRenderer {
public:
    SpriteRenderer(); // Custom constructor needed for unique_ptr with forward declaration
    ~SpriteRenderer(); // Custom destructor needed for unique_ptr with forward declaration

    bool initialize(const Renderer& renderer, const TextureManager& texture_manager);
    virtual void beginFrame();
    virtual void drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size);
    virtual void endFrame();
    
    // Alpha blending support
    void enableAlphaBlending();
    void disableAlphaBlending();
    
    // Depth testing support
    void enableDepthTesting();
    void disableDepthTesting();
    void enableDepthWrites();
    void disableDepthWrites();
    
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
};

} // namespace d2::rendering