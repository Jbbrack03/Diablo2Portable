#include "rendering/vertex_buffer.h"
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
// Mock OpenGL implementation for testing
#include <random>
#include <cstdint>

// Mock OpenGL constants
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_NO_ERROR 0

// Mock OpenGL types
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef void GLvoid;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// Mock OpenGL functions that return realistic buffer IDs
extern "C" {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dis(1000, 999999);
    
    void glGenBuffers(GLsizei n, GLuint* buffers) {
        for (int i = 0; i < n; i++) {
            buffers[i] = dis(gen);  // Random buffer ID
        }
    }
    
    void glBindBuffer(GLenum target, GLuint buffer) {
        (void)target; (void)buffer;
    }
    
    void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) {
        (void)target; (void)size; (void)data; (void)usage;
    }
    
    void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data) {
        (void)target; (void)offset; (void)size; (void)data;
    }
    
    void glDeleteBuffers(GLsizei n, const GLuint* buffers) {
        (void)n; (void)buffers;
    }
    
    GLenum glGetError() {
        return GL_NO_ERROR;
    }
}
#endif

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
    
    // Release any existing buffer
    release();
    
    vertexCount_ = vertices.size();
    
    // Create OpenGL vertex buffer
    glGenBuffers(1, &bufferId_);
    if (bufferId_ == 0) {
        return false;
    }
    
    // Bind buffer and upload data
    glBindBuffer(GL_ARRAY_BUFFER, bufferId_);
    glBufferData(GL_ARRAY_BUFFER, 
                 vertices.size() * sizeof(SpriteVertex),
                 vertices.data(),
                 GL_STATIC_DRAW);
    
    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        release();
        return false;
    }
    
    return true;
}

bool VertexBuffer::update(const std::vector<SpriteVertex>& vertices) {
    if (!isValid() || vertices.empty()) {
        return false;
    }
    
    // Bind buffer and update data
    glBindBuffer(GL_ARRAY_BUFFER, bufferId_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
                    vertices.size() * sizeof(SpriteVertex),
                    vertices.data());
    
    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        return false;
    }
    
    vertexCount_ = vertices.size();
    return true;
}

void VertexBuffer::bind() const {
    if (!isValid()) {
        return;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, bufferId_);
}

void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::release() {
    if (bufferId_ != 0) {
        glDeleteBuffers(1, &bufferId_);
        bufferId_ = 0;
        vertexCount_ = 0;
    }
}

} // namespace d2::rendering