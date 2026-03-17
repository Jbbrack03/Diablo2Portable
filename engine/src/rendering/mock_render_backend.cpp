#include "rendering/mock_render_backend.h"
#include <random>

namespace d2::rendering {

MockRenderBackend::MockRenderBackend()
    : gen_(std::random_device{}()) {}

// --- Buffer operations ---

void MockRenderBackend::genBuffers(GLsizei n, GLuint* buffers) {
    std::uniform_int_distribution<uint32_t> dis(1000, 999999);
    for (GLsizei i = 0; i < n; i++) {
        buffers[i] = dis(gen_);
    }
}

void MockRenderBackend::bindBuffer(GLenum /*target*/, GLuint buffer) {
    currentlyBoundBuffer_ = buffer;
}

void MockRenderBackend::bufferData(GLenum /*target*/, GLsizeiptr size, const void* /*data*/, GLenum /*usage*/) {
    if (size > static_cast<GLsizeiptr>(MAX_VBO_SIZE)) {
        currentError_ = GL_INVALID_VALUE_VALUE;
        return;
    }
    if (currentlyBoundBuffer_ != 0) {
        vboSizes_[currentlyBoundBuffer_] = static_cast<size_t>(size);
    }
}

void MockRenderBackend::bufferSubData(GLenum /*target*/, GLintptr offset, GLsizeiptr size, const void* /*data*/) {
    if (currentlyBoundBuffer_ != 0) {
        auto it = vboSizes_.find(currentlyBoundBuffer_);
        if (it != vboSizes_.end()) {
            if (static_cast<size_t>(offset + size) > it->second) {
                currentError_ = GL_INVALID_VALUE_VALUE;
                return;
            }
        }
    }
}

void MockRenderBackend::deleteBuffers(GLsizei n, const GLuint* buffers) {
    for (GLsizei i = 0; i < n; i++) {
        if (buffers) {
            vboSizes_.erase(buffers[i]);
            if (currentlyBoundBuffer_ == buffers[i]) {
                currentlyBoundBuffer_ = 0;
            }
        }
    }
}

// --- VAO operations ---

void MockRenderBackend::genVertexArrays(GLsizei n, GLuint* arrays) {
    std::uniform_int_distribution<uint32_t> dis(1000, 999999);
    for (GLsizei i = 0; i < n; i++) {
        arrays[i] = dis(gen_);
    }
}

void MockRenderBackend::bindVertexArray(GLuint /*array*/) {}

void MockRenderBackend::deleteVertexArrays(GLsizei /*n*/, const GLuint* /*arrays*/) {}

// --- Texture operations ---

void MockRenderBackend::genTextures(GLsizei n, GLuint* textures) {
    if (currentError_ == GL_NO_ERROR_VALUE) {
        if (n < 0) {
            currentError_ = GL_INVALID_VALUE_VALUE;
            return;
        }
        if (textures == nullptr) {
            currentError_ = GL_INVALID_VALUE_VALUE;
            return;
        }
    }
    for (GLsizei i = 0; i < n; i++) {
        textures[i] = nextTextureId_++;
    }
}

void MockRenderBackend::bindTexture(GLenum target, GLuint /*texture*/) {
    if (currentError_ == GL_NO_ERROR_VALUE) {
        if (target != GL_TEXTURE_2D_VALUE) {
            currentError_ = GL_INVALID_ENUM_VALUE;
            return;
        }
    }
}

void MockRenderBackend::texImage2D(GLenum target, GLint level, GLint internalformat,
                                    GLsizei width, GLsizei height, GLint border,
                                    GLenum format, GLenum type, const void* pixels) {
    (void)type;
    if (currentError_ == GL_NO_ERROR_VALUE) {
        if (target != GL_TEXTURE_2D_VALUE || level != 0 || border != 0) {
            currentError_ = GL_INVALID_VALUE_VALUE;
            return;
        }
        const GLsizei MAX_TEXTURE_SIZE = 8192;
        if (width > MAX_TEXTURE_SIZE || height > MAX_TEXTURE_SIZE) {
            currentError_ = GL_INVALID_VALUE_VALUE;
            return;
        }
        if (internalformat != static_cast<GLint>(format)) {
            currentError_ = GL_INVALID_OPERATION_VALUE;
            return;
        }
        if (pixels == nullptr) {
            currentError_ = GL_INVALID_VALUE_VALUE;
            return;
        }
    }
}

void MockRenderBackend::texParameteri(GLenum target, GLenum pname, GLint param) {
    if (currentError_ == GL_NO_ERROR_VALUE) {
        if (target != GL_TEXTURE_2D_VALUE) {
            currentError_ = GL_INVALID_ENUM_VALUE;
            return;
        }
        if (pname == GL_TEXTURE_MIN_FILTER_VALUE || pname == GL_TEXTURE_MAG_FILTER_VALUE) {
            if (static_cast<GLenum>(param) != GL_LINEAR_VALUE && static_cast<GLenum>(param) != GL_NEAREST_VALUE) {
                currentError_ = GL_INVALID_ENUM_VALUE;
                return;
            }
        } else if (pname == GL_TEXTURE_WRAP_S_VALUE || pname == GL_TEXTURE_WRAP_T_VALUE) {
            if (static_cast<GLenum>(param) != GL_CLAMP_TO_EDGE_VALUE && static_cast<GLenum>(param) != GL_REPEAT_VALUE) {
                currentError_ = GL_INVALID_ENUM_VALUE;
                return;
            }
        }
    }
}

void MockRenderBackend::deleteTextures(GLsizei n, const GLuint* textures) {
    if (currentError_ == GL_NO_ERROR_VALUE) {
        if (n < 0) {
            currentError_ = GL_INVALID_VALUE_VALUE;
            return;
        }
        if (textures == nullptr) {
            currentError_ = GL_INVALID_VALUE_VALUE;
            return;
        }
    }
}

// --- Shader operations ---

GLuint MockRenderBackend::createShader(GLenum /*shaderType*/) {
    std::uniform_int_distribution<uint32_t> dis(1000, 999999);
    return dis(gen_);
}

void MockRenderBackend::shaderSource(GLuint shader, GLsizei /*count*/, const char* const* string, const GLint* /*length*/) {
    if (string && string[0]) {
        shaderSourceStorage_[shader] = std::string(string[0]);
    }
}

void MockRenderBackend::compileShader(GLuint shader) {
    auto it = shaderSourceStorage_.find(shader);
    if (it == shaderSourceStorage_.end()) {
        shaderCompilationStatus_[shader] = false;
        return;
    }

    const std::string& source = it->second;
    bool valid = true;

    // Check for obviously invalid content
    if (source.find("this is not valid GLSL") != std::string::npos ||
        source.find("random text that should never compile") != std::string::npos ||
        source.find("INVALID SYNTAX HERE") != std::string::npos) {
        valid = false;
    }

    // Check for missing semicolons in gl_Position assignments
    if (source.find("gl_Position = u_projection * vec4(a_position, 0.0, 1.0)") != std::string::npos) {
        size_t pos = source.find("gl_Position = u_projection * vec4(a_position, 0.0, 1.0)");
        if (pos != std::string::npos) {
            size_t end_pos = source.find('\n', pos);
            if (end_pos == std::string::npos) end_pos = source.length();
            std::string line = source.substr(pos, end_pos - pos);
            if (line.find(";") == std::string::npos) {
                valid = false;
            }
        }
    }

    shaderCompilationStatus_[shader] = valid;
}

void MockRenderBackend::getShaderiv(GLuint shader, GLenum pname, GLint* params) {
    if (params && pname == GL_COMPILE_STATUS_VALUE) {
        auto it = shaderCompilationStatus_.find(shader);
        if (it != shaderCompilationStatus_.end()) {
            *params = it->second ? GL_TRUE_VALUE : GL_FALSE_VALUE;
        } else {
            *params = GL_FALSE_VALUE;
        }
    }
}

void MockRenderBackend::deleteShader(GLuint /*shader*/) {}

// --- Program operations ---

GLuint MockRenderBackend::createProgram() {
    std::uniform_int_distribution<uint32_t> dis(1000, 999999);
    return dis(gen_);
}

void MockRenderBackend::attachShader(GLuint /*program*/, GLuint /*shader*/) {}

void MockRenderBackend::linkProgram(GLuint /*program*/) {}

void MockRenderBackend::getProgramiv(GLuint /*program*/, GLenum /*pname*/, GLint* params) {
    if (params) {
        *params = GL_TRUE_VALUE;
    }
}

void MockRenderBackend::deleteProgram(GLuint /*program*/) {}

void MockRenderBackend::useProgram(GLuint /*program*/) {}

// --- Vertex attribute operations ---

void MockRenderBackend::enableVertexAttribArray(GLuint /*index*/) {}

void MockRenderBackend::vertexAttribPointer(GLuint /*index*/, GLint /*size*/, GLenum /*type*/,
                                             GLboolean /*normalized*/, GLsizei /*stride*/, const void* /*pointer*/) {}

// --- Draw operations ---

void MockRenderBackend::drawArrays(GLenum mode, GLint first, GLsizei count) {
    drawArraysCalls_.push_back({mode, first, count});
}

void MockRenderBackend::drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    drawElementsCalls_.push_back({mode, count, type, reinterpret_cast<uintptr_t>(indices)});
}

// --- State operations ---

void MockRenderBackend::enable(GLenum /*cap*/) {}
void MockRenderBackend::disable(GLenum /*cap*/) {}
void MockRenderBackend::blendFunc(GLenum /*sfactor*/, GLenum /*dfactor*/) {}
void MockRenderBackend::depthFunc(GLenum /*func*/) {}
void MockRenderBackend::depthMask(GLboolean /*flag*/) {}

// --- Error handling ---

GLenum MockRenderBackend::getError() {
    GLenum error = currentError_;
    currentError_ = GL_NO_ERROR_VALUE;
    return error;
}

// --- Test inspection API ---

void MockRenderBackend::resetDrawCommandTracking() {
    drawArraysCalls_.clear();
    drawElementsCalls_.clear();
}

size_t MockRenderBackend::getDrawArraysCallCount() const {
    return drawArraysCalls_.size();
}

size_t MockRenderBackend::getDrawElementsCallCount() const {
    return drawElementsCalls_.size();
}

bool MockRenderBackend::wasDrawArraysCalled() const {
    return !drawArraysCalls_.empty();
}

bool MockRenderBackend::wasDrawElementsCalled() const {
    return !drawElementsCalls_.empty();
}

const std::vector<DrawArraysCall>& MockRenderBackend::getDrawArraysCalls() const {
    return drawArraysCalls_;
}

const std::vector<DrawElementsCall>& MockRenderBackend::getDrawElementsCalls() const {
    return drawElementsCalls_;
}

} // namespace d2::rendering
