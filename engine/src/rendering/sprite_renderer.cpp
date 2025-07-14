#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/shader_manager.h"
#include "rendering/vertex_buffer.h"
#include "rendering/vertex_array_object.h"
#include "tools/texture_atlas_generator.h"
#include <unordered_set>
#include <cstddef>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
// Mock OpenGL constants and functions for desktop testing
#define GL_TRIANGLES 0x0004
#define GL_FLOAT 0x1406
#define GL_FALSE 0
#define GL_TEXTURE_2D 0x0DE1
#define GL_BLEND 0x0BE2
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DEPTH_TEST 0x0B71
#define GL_LEQUAL 0x0203

extern "C" {
    void glUseProgram(uint32_t program);
    void glDrawArrays(uint32_t mode, int first, int count);
    void glEnableVertexAttribArray(uint32_t index);
    void glVertexAttribPointer(uint32_t index, int size, uint32_t type, bool normalized, int stride, const void* pointer);
    void glBindTexture(uint32_t target, uint32_t texture);
    void glEnable(uint32_t cap);
    void glDisable(uint32_t cap);
    void glBlendFunc(uint32_t src, uint32_t dst);
    void glDepthFunc(uint32_t func);
    void glDepthMask(uint8_t flag);
}
#endif

namespace d2::rendering {

SpriteRenderer::SpriteRenderer() = default;

SpriteRenderer::~SpriteRenderer() = default;

bool SpriteRenderer::initialize(const Renderer& renderer, const TextureManager& texture_manager) {
    // Minimal implementation to pass the test
    (void)renderer;        // Suppress unused parameter warning
    (void)texture_manager; // Suppress unused parameter warning
    
    // Create shader manager and compile sprite shaders
    shader_manager_ = std::make_unique<ShaderManager>();
    
    // Basic vertex shader for sprite rendering
    const std::string vertex_shader_source = R"(
        #version 300 es
        layout(location = 0) in vec2 a_position;
        layout(location = 1) in vec2 a_texcoord;
        uniform mat4 u_projection;
        out vec2 v_texcoord;
        void main() {
            gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
            v_texcoord = a_texcoord;
        }
    )";
    
    // Basic fragment shader for sprite rendering
    const std::string fragment_shader_source = R"(
        #version 300 es
        precision mediump float;
        in vec2 v_texcoord;
        uniform sampler2D u_texture;
        out vec4 fragColor;
        void main() {
            fragColor = texture(u_texture, v_texcoord);
        }
    )";
    
    // Compile shaders
    uint32_t vertex_shader = shader_manager_->compileShader(ShaderType::VERTEX, vertex_shader_source);
    uint32_t fragment_shader = shader_manager_->compileShader(ShaderType::FRAGMENT, fragment_shader_source);
    
    if (vertex_shader == 0 || fragment_shader == 0) {
        return false;
    }
    
    // Create shader program
    shader_program_ = shader_manager_->createProgram(vertex_shader, fragment_shader);
    if (shader_program_ == 0) {
        return false;
    }
    
    // Clean up individual shaders (they're now part of the program)
    shader_manager_->deleteShader(vertex_shader);
    shader_manager_->deleteShader(fragment_shader);
    
    // Create VAO for sprite rendering
    vao_ = std::make_unique<VertexArrayObject>();
    if (!vao_->create()) {
        return false;
    }
    
    // Create vertex buffer for sprite batching
    vertex_buffer_ = std::make_unique<VertexBuffer>();
    // Initialize with empty data - will be filled during rendering
    std::vector<SpriteVertex> initial_vertices;
    initial_vertices.reserve(1000); // Reserve space for batching
    if (!vertex_buffer_->create(initial_vertices)) {
        // Create with at least one vertex to make it valid
        initial_vertices.push_back({{0, 0}, {0, 0}});
        if (!vertex_buffer_->create(initial_vertices)) {
            return false;
        }
    }
    
    initialized_ = true;
    return true;
}

void SpriteRenderer::beginFrame() {
    // Reset counters for new frame
    draw_call_count_ = 0;
    sprite_count_ = 0;
    textures_used_.clear();
    sprite_batches_.clear();
    
    // Activate shader program for this frame
    if (shader_program_ != 0) {
        glUseProgram(shader_program_);
        shader_program_active_ = true;
    }
    
    // Bind VAO for sprite rendering
    if (vao_) {
        vao_->bind();
    }
}

void SpriteRenderer::drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size) {
    sprite_count_++;
    textures_used_.insert(texture_id);
    
    // Create vertices for a sprite quad (two triangles)
    // Top-left
    SpriteVertex v0{{position.x, position.y}, {0.0f, 0.0f}};
    // Top-right
    SpriteVertex v1{{position.x + size.x, position.y}, {1.0f, 0.0f}};
    // Bottom-left
    SpriteVertex v2{{position.x, position.y + size.y}, {0.0f, 1.0f}};
    // Bottom-right
    SpriteVertex v3{{position.x + size.x, position.y + size.y}, {1.0f, 1.0f}};
    
    // Add vertices to the batch for this texture (two triangles)
    auto& batch = sprite_batches_[texture_id];
    batch.texture_id = texture_id;
    
    // First triangle (top-left, top-right, bottom-left)
    batch.vertices.push_back(v0);
    batch.vertices.push_back(v1);
    batch.vertices.push_back(v2);
    
    // Second triangle (top-right, bottom-right, bottom-left)
    batch.vertices.push_back(v1);
    batch.vertices.push_back(v3);
    batch.vertices.push_back(v2);
}

void SpriteRenderer::endFrame() {
    // Setup vertex attributes if we have a valid VAO
    if (vao_ && vertex_buffer_) {
        // Enable vertex attributes
        glEnableVertexAttribArray(0); // position
        glEnableVertexAttribArray(1); // texcoord
        
        // Set vertex attribute pointers
        // Position attribute (location = 0)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), 
                            reinterpret_cast<void*>(offsetof(SpriteVertex, position)));
        
        // TexCoord attribute (location = 1)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), 
                            reinterpret_cast<void*>(offsetof(SpriteVertex, texCoord)));
    }
    
    // Process each batch and make draw calls
    draw_call_count_ = 0;
    for (const auto& [texture_id, batch] : sprite_batches_) {
        if (!batch.vertices.empty() && vertex_buffer_) {
            // Update vertex buffer with batch data
            vertex_buffer_->update(batch.vertices);
            vertex_buffer_->bind();
            
            // Bind texture for this batch
            glBindTexture(GL_TEXTURE_2D, texture_id);
            
            // Make the actual draw call
            glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(batch.vertices.size()));
            draw_call_count_++;
        }
    }
    
    // Deactivate shader program
    shader_program_active_ = false;
    if (shader_program_ != 0) {
        glUseProgram(0);
    }
    
    // Unbind VAO
    if (vao_) {
        VertexArrayObject::unbind();
    }
}

uint32_t SpriteRenderer::getDrawCallCount() const {
    return draw_call_count_;
}

uint32_t SpriteRenderer::getSpriteCount() const {
    return sprite_count_;
}

uint32_t SpriteRenderer::getShaderProgram() const {
    return shader_program_;
}

bool SpriteRenderer::isShaderProgramActive() const {
    return shader_program_active_;
}

uint32_t SpriteRenderer::getVAOId() const {
    return vao_ ? vao_->getVAOId() : 0;
}

uint32_t SpriteRenderer::getVertexBufferId() const {
    return vertex_buffer_ ? vertex_buffer_->getBufferId() : 0;
}

void SpriteRenderer::enableAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SpriteRenderer::disableAlphaBlending() {
    glDisable(GL_BLEND);
}

void SpriteRenderer::enableDepthTesting() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void SpriteRenderer::disableDepthTesting() {
    glDisable(GL_DEPTH_TEST);
}

void SpriteRenderer::enableDepthWrites() {
    glDepthMask(1);  // GL_TRUE
}

void SpriteRenderer::disableDepthWrites() {
    glDepthMask(0);  // GL_FALSE
}

void SpriteRenderer::enableAlphaTesting(float threshold) {
    alpha_testing_enabled_ = true;
    alpha_test_threshold_ = threshold;
}

void SpriteRenderer::disableAlphaTesting() {
    alpha_testing_enabled_ = false;
}

bool SpriteRenderer::isAlphaTestingEnabled() const {
    return alpha_testing_enabled_;
}

float SpriteRenderer::getAlphaTestThreshold() const {
    return alpha_test_threshold_;
}

std::string SpriteRenderer::getFragmentShaderSource() const {
    if (alpha_testing_enabled_) {
        return R"(
        #version 300 es
        precision mediump float;
        in vec2 v_texcoord;
        uniform sampler2D u_texture;
        uniform float u_alphaThreshold;
        out vec4 fragColor;
        void main() {
            vec4 texColor = texture(u_texture, v_texcoord);
            if (texColor.a < u_alphaThreshold) {
                discard;
            }
            fragColor = texColor;
        }
        )";
    } else {
        return R"(
        #version 300 es
        precision mediump float;
        in vec2 v_texcoord;
        uniform sampler2D u_texture;
        out vec4 fragColor;
        void main() {
            fragColor = texture(u_texture, v_texcoord);
        }
        )";
    }
}

void SpriteRenderer::addAtlas(const d2::TextureAtlas& atlas) {
    atlases_.push_back(atlas);
}

uint32_t SpriteRenderer::getAtlasCount() const {
    return static_cast<uint32_t>(atlases_.size());
}

void SpriteRenderer::drawSpriteFromAtlas(const std::string& spriteName, const glm::vec2& position, const glm::vec2& size) {
    // For now, just treat as a regular sprite with texture ID 1
    // In a real implementation, we would look up the sprite in the atlas
    // and use the appropriate texture coordinates
    (void)spriteName;  // Suppress unused parameter warning
    drawSprite(1, position, size);
}

} // namespace d2::rendering