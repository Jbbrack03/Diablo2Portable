#include "rendering/vertex_array_object.h"
#include "rendering/render_context.h"
#include "rendering/render_backend.h"

namespace d2::rendering {

VertexArrayObject::~VertexArrayObject() {
    release();
}

VertexArrayObject::VertexArrayObject(VertexArrayObject&& other) noexcept
    : vaoId_(other.vaoId_) {
    other.vaoId_ = 0;
}

VertexArrayObject& VertexArrayObject::operator=(VertexArrayObject&& other) noexcept {
    if (this != &other) {
        release();
        vaoId_ = other.vaoId_;
        other.vaoId_ = 0;
    }
    return *this;
}

bool VertexArrayObject::create() {
    // Release any existing VAO
    release();

    auto* backend = RenderContext::getBackend();
    if (!backend) return false;

    // Generate a new VAO
    backend->genVertexArrays(1, &vaoId_);
    if (vaoId_ == 0) {
        return false;
    }

    // Check for OpenGL errors
    GLenum error = backend->getError();
    if (error != GL_NO_ERROR_VALUE) {
        release();
        return false;
    }

    return true;
}

void VertexArrayObject::bind() const {
    if (vaoId_ != 0) {
        auto* backend = RenderContext::getBackend();
        if (backend) {
            backend->bindVertexArray(vaoId_);
        }
    }
}

void VertexArrayObject::unbind() {
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->bindVertexArray(0);
    }
}

void VertexArrayObject::release() {
    if (vaoId_ != 0) {
        auto* backend = RenderContext::getBackend();
        if (backend) {
            backend->deleteVertexArrays(1, &vaoId_);
        }
        vaoId_ = 0;
    }
}

} // namespace d2::rendering
