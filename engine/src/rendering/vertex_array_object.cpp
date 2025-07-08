#include "rendering/vertex_array_object.h"

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
// Mock OpenGL constants
#define GL_NO_ERROR 0

// Mock OpenGL types
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;

// External mock OpenGL functions
extern "C" {
    void glGenVertexArrays(GLsizei n, GLuint* arrays);
    void glBindVertexArray(GLuint array);
    void glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
    GLenum glGetError();
}
#endif

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
    
    // Generate a new VAO
    glGenVertexArrays(1, &vaoId_);
    if (vaoId_ == 0) {
        return false;
    }
    
    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        release();
        return false;
    }
    
    return true;
}

void VertexArrayObject::bind() const {
    if (vaoId_ != 0) {
        glBindVertexArray(vaoId_);
    }
}

void VertexArrayObject::unbind() {
    glBindVertexArray(0);
}

void VertexArrayObject::release() {
    if (vaoId_ != 0) {
        glDeleteVertexArrays(1, &vaoId_);
        vaoId_ = 0;
    }
}

} // namespace d2::rendering