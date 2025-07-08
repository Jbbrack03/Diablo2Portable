#ifndef __ANDROID__
// Mock OpenGL implementation for testing

#include <random>
#include <cstdint>

// Mock OpenGL types
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef void GLvoid;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// Mock OpenGL constants
#define GL_NO_ERROR 0

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
    
    void glGenVertexArrays(GLsizei n, GLuint* arrays) {
        for (int i = 0; i < n; i++) {
            arrays[i] = dis(gen);  // Random VAO ID
        }
    }
    
    void glBindVertexArray(GLuint array) {
        (void)array;
    }
    
    void glDeleteVertexArrays(GLsizei n, const GLuint* arrays) {
        (void)n; (void)arrays;
    }
    
    GLenum glGetError() {
        return GL_NO_ERROR;
    }
    
    // Shader and draw call functions
    void glUseProgram(GLuint program) {
        (void)program;
    }
    
    void glDrawArrays(GLenum mode, int first, GLsizei count) {
        (void)mode; (void)first; (void)count;
    }
    
    void glEnableVertexAttribArray(GLuint index) {
        (void)index;
    }
    
    void glVertexAttribPointer(GLuint index, int size, GLenum type, bool normalized, GLsizei stride, const void* pointer) {
        (void)index; (void)size; (void)type; (void)normalized; (void)stride; (void)pointer;
    }
}

#endif // __ANDROID__