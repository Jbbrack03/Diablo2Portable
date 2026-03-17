#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/shader_manager.h"
#include "rendering/vertex_buffer.h"
#include "rendering/vertex_array_object.h"
#include "rendering/vertex_buffer_pool.h"
#include "rendering/render_context.h"
#include "rendering/render_backend.h"
#include "tools/texture_atlas_generator.h"
#include <unordered_set>
#include <cstddef>

namespace d2::rendering {

SpriteRenderer::SpriteRenderer() = default;

SpriteRenderer::~SpriteRenderer() = default;

bool SpriteRenderer::initialize(const Renderer& renderer, const TextureManager& texture_manager) {
    (void)renderer;
    (void)texture_manager;

    // Create shader manager and compile sprite shaders
    shader_manager_ = std::make_unique<ShaderManager>();

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

    uint32_t vertex_shader = shader_manager_->compileShader(ShaderType::VERTEX, vertex_shader_source);
    uint32_t fragment_shader = shader_manager_->compileShader(ShaderType::FRAGMENT, fragment_shader_source);

    if (vertex_shader == 0 || fragment_shader == 0) {
        return false;
    }

    shader_program_ = shader_manager_->createProgram(vertex_shader, fragment_shader);
    if (shader_program_ == 0) {
        return false;
    }

    shader_manager_->deleteShader(vertex_shader);
    shader_manager_->deleteShader(fragment_shader);

    // Create VAO for sprite rendering
    vao_ = std::make_unique<VertexArrayObject>();
    if (!vao_->create()) {
        return false;
    }

    // Create vertex buffer for sprite batching
    vertex_buffer_ = std::make_unique<VertexBuffer>();
    std::vector<SpriteVertex> initial_vertices;
    initial_vertices.reserve(1000);
    if (!vertex_buffer_->create(initial_vertices)) {
        initial_vertices.push_back({{0, 0}, {0, 0}});
        if (!vertex_buffer_->create(initial_vertices)) {
            return false;
        }
    }

    initialized_ = true;
    return true;
}

void SpriteRenderer::beginFrame() {
    draw_call_count_ = 0;
    sprite_count_ = 0;
    textures_used_.clear();
    sprite_batches_.clear();

    auto* backend = RenderContext::getBackend();

    if (shader_program_ != 0 && backend) {
        backend->useProgram(shader_program_);
        shader_program_active_ = true;
    }

    if (vao_) {
        vao_->bind();
    }
}

void SpriteRenderer::drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size) {
    sprite_count_++;
    textures_used_.insert(texture_id);

    SpriteVertex v0{{position.x, position.y}, {0.0f, 0.0f}};
    SpriteVertex v1{{position.x + size.x, position.y}, {1.0f, 0.0f}};
    SpriteVertex v2{{position.x, position.y + size.y}, {0.0f, 1.0f}};
    SpriteVertex v3{{position.x + size.x, position.y + size.y}, {1.0f, 1.0f}};

    auto& batch = sprite_batches_[texture_id];
    batch.texture_id = texture_id;

    batch.vertices.push_back(v0);
    batch.vertices.push_back(v1);
    batch.vertices.push_back(v2);

    batch.vertices.push_back(v1);
    batch.vertices.push_back(v3);
    batch.vertices.push_back(v2);
}

void SpriteRenderer::endFrame() {
    auto* backend = RenderContext::getBackend();

    if (vao_ && vertex_buffer_ && backend) {
        backend->enableVertexAttribArray(0);
        backend->enableVertexAttribArray(1);

        backend->vertexAttribPointer(0, 2, GL_FLOAT_VALUE, GL_FALSE_VALUE, sizeof(SpriteVertex),
                            reinterpret_cast<void*>(offsetof(SpriteVertex, position)));

        backend->vertexAttribPointer(1, 2, GL_FLOAT_VALUE, GL_FALSE_VALUE, sizeof(SpriteVertex),
                            reinterpret_cast<void*>(offsetof(SpriteVertex, texCoord)));
    }

    draw_call_count_ = 0;
    for (const auto& [texture_id, batch] : sprite_batches_) {
        if (!batch.vertices.empty() && vertex_buffer_ && backend) {
            vertex_buffer_->update(batch.vertices);
            vertex_buffer_->bind();

            backend->bindTexture(GL_TEXTURE_2D_VALUE, texture_id);

            backend->drawArrays(GL_TRIANGLES_VALUE, 0, static_cast<GLsizei>(batch.vertices.size()));
            draw_call_count_++;
        }
    }

    shader_program_active_ = false;
    if (shader_program_ != 0 && backend) {
        backend->useProgram(0);
    }

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
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->enable(GL_BLEND_VALUE);
        backend->blendFunc(GL_SRC_ALPHA_VALUE, GL_ONE_MINUS_SRC_ALPHA_VALUE);
    }
}

void SpriteRenderer::disableAlphaBlending() {
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->disable(GL_BLEND_VALUE);
    }
}

void SpriteRenderer::enableDepthTesting() {
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->enable(GL_DEPTH_TEST_VALUE);
        backend->depthFunc(GL_LEQUAL_VALUE);
    }
}

void SpriteRenderer::disableDepthTesting() {
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->disable(GL_DEPTH_TEST_VALUE);
    }
}

void SpriteRenderer::enableDepthWrites() {
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->depthMask(1);
    }
}

void SpriteRenderer::disableDepthWrites() {
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->depthMask(0);
    }
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
    (void)spriteName;

    uint32_t texture_id = 1;
    auto& batch = sprite_batches_[texture_id];
    if (batch.texture_id == 0) {
        batch.texture_id = texture_id;
    }

    SpriteVertex vertices[4];
    vertices[0].position = position;
    vertices[1].position = {position.x + size.x, position.y};
    vertices[2].position = {position.x + size.x, position.y + size.y};
    vertices[3].position = {position.x, position.y + size.y};

    batch.vertices.insert(batch.vertices.end(), vertices, vertices + 4);

    sprite_count_++;
}

void SpriteRenderer::beginBatch() {
    sprite_batches_.clear();
    draw_call_count_ = 0;
    sprite_count_ = 0;
}

void SpriteRenderer::endBatch() {
    for (const auto& [texture_id, batch] : sprite_batches_) {
        if (!batch.vertices.empty()) {
            if (vertex_buffer_pool_ && batch.vertices.size() > 100) {
                auto pooled_buffer = vertex_buffer_pool_->acquire(batch.vertices.size());
                if (pooled_buffer) {
                    pooled_buffer->update(batch.vertices);
                    vertex_buffer_pool_->release(pooled_buffer);
                }
            }
            draw_call_count_++;
        }
    }
}

void SpriteRenderer::setVertexBufferPool(std::shared_ptr<VertexBufferPool> pool) {
    vertex_buffer_pool_ = pool;
}

} // namespace d2::rendering
