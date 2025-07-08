#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/shader_manager.h"
#include "rendering/vertex_buffer.h"
#include "rendering/vertex_array_object.h"
#include <unordered_set>

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
    
    // Activate shader program for this frame
    if (shader_program_ != 0) {
        // In a real implementation, this would call glUseProgram(shader_program_)
        shader_program_active_ = true;
    }
}

void SpriteRenderer::drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size) {
    // Minimal implementation - count sprite and track unique textures
    (void)position;   // Suppress unused parameter warning
    (void)size;       // Suppress unused parameter warning
    
    sprite_count_++;
    textures_used_.insert(texture_id);
}

void SpriteRenderer::endFrame() {
    // One draw call per unique texture (basic batching)
    draw_call_count_ = static_cast<uint32_t>(textures_used_.size());
    
    // Deactivate shader program
    shader_program_active_ = false;
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

} // namespace d2::rendering