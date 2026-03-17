#include "rendering/vertex_buffer.h"
#include "rendering/render_context.h"
#include "rendering/render_backend.h"

namespace d2::rendering {

VertexBuffer::VertexBuffer() = default;

VertexBuffer::~VertexBuffer() {
    release();
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : bufferId_(other.bufferId_)
    , vertexCount_(other.vertexCount_) {
    other.bufferId_ = 0;
    other.vertexCount_ = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
    if (this != &other) {
        release();
        bufferId_ = other.bufferId_;
        vertexCount_ = other.vertexCount_;
        other.bufferId_ = 0;
        other.vertexCount_ = 0;
    }
    return *this;
}

bool VertexBuffer::create(const std::vector<SpriteVertex>& vertices) {
    if (vertices.empty()) {
        return false;
    }

    auto* backend = RenderContext::getBackend();
    if (!backend) return false;

    // Release any existing buffer
    release();

    vertexCount_ = vertices.size();

    // Create OpenGL vertex buffer
    backend->genBuffers(1, &bufferId_);
    if (bufferId_ == 0) {
        return false;
    }

    // Bind buffer and upload data
    backend->bindBuffer(GL_ARRAY_BUFFER_VALUE, bufferId_);
    backend->bufferData(GL_ARRAY_BUFFER_VALUE,
                 vertices.size() * sizeof(SpriteVertex),
                 vertices.data(),
                 GL_STATIC_DRAW_VALUE);

    // Check for OpenGL errors
    GLenum error = backend->getError();
    if (error != GL_NO_ERROR_VALUE) {
        release();
        return false;
    }

    return true;
}

bool VertexBuffer::update(const std::vector<SpriteVertex>& vertices) {
    if (!isValid() || vertices.empty()) {
        return false;
    }

    auto* backend = RenderContext::getBackend();
    if (!backend) return false;

    // Bind buffer and update data
    backend->bindBuffer(GL_ARRAY_BUFFER_VALUE, bufferId_);
    backend->bufferSubData(GL_ARRAY_BUFFER_VALUE, 0,
                    vertices.size() * sizeof(SpriteVertex),
                    vertices.data());

    // Check for OpenGL errors
    GLenum error = backend->getError();
    if (error != GL_NO_ERROR_VALUE) {
        return false;
    }

    vertexCount_ = vertices.size();
    return true;
}

void VertexBuffer::bind() const {
    if (!isValid()) {
        return;
    }

    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->bindBuffer(GL_ARRAY_BUFFER_VALUE, bufferId_);
    }
}

void VertexBuffer::unbind() {
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->bindBuffer(GL_ARRAY_BUFFER_VALUE, 0);
    }
}

void VertexBuffer::release() {
    if (bufferId_ != 0) {
        auto* backend = RenderContext::getBackend();
        if (backend) {
            backend->deleteBuffers(1, &bufferId_);
        }
        bufferId_ = 0;
        vertexCount_ = 0;
    }
}

} // namespace d2::rendering
