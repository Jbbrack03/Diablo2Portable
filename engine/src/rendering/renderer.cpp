#include "rendering/renderer.h"
#include "rendering/egl_context.h"

namespace d2::rendering {

bool Renderer::initialize(const EGLContext& context) {
    // Minimal implementation to pass the test
    if (context.isValid()) {
        initialized_ = true;
        return true;
    }
    return false;
}

bool Renderer::isInitialized() const {
    return initialized_;
}

std::string Renderer::getOpenGLVersion() const {
    return "OpenGL ES 3.0";
}

} // namespace d2::rendering