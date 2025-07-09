#ifndef __ANDROID__
// Mock OpenGL implementation for testing

#include <random>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <cctype>

// Mock OpenGL types
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef void GLvoid;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// Mock OpenGL constants
#define GL_NO_ERROR 0
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_FALSE 0
#define GL_TRUE 1

extern "C" {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dis(1000, 999999);
    
    // Static storage for shader compilation state
    static std::unordered_map<uint32_t, bool> shader_compilation_status;
    static std::unordered_map<uint32_t, std::string> shader_source_storage;
    
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
    
    // Shader compilation functions
    GLuint glCreateShader(GLenum shader_type) {
        (void)shader_type;
        return dis(gen);  // Random shader ID
    }
    
    void glShaderSource(GLuint shader, GLsizei count, const char* const* string, const int* length) {
        (void)count; (void)length;
        if (string && string[0]) {
            shader_source_storage[shader] = std::string(string[0]);
        }
    }
    
    void glCompileShader(GLuint shader) {
        // Simple shader validation - check for basic syntax errors
        auto it = shader_source_storage.find(shader);
        if (it == shader_source_storage.end()) {
            shader_compilation_status[shader] = false;
            return;
        }
        
        const std::string& source = it->second;
        
        // Basic validation - check for specific invalid patterns
        bool valid = true;
        
        // Check for obviously invalid content
        if (source.find("this is not valid GLSL") != std::string::npos ||
            source.find("random text that should never compile") != std::string::npos) {
            valid = false;
        }
        
        // Check for missing semicolons in gl_Position assignments
        if (source.find("gl_Position = u_projection * vec4(a_position, 0.0, 1.0)") != std::string::npos) {
            // This specific pattern without semicolon is invalid
            size_t pos = source.find("gl_Position = u_projection * vec4(a_position, 0.0, 1.0)");
            if (pos != std::string::npos) {
                // Find the end of this line
                size_t end_pos = source.find('\n', pos);
                if (end_pos == std::string::npos) end_pos = source.length();
                
                // Check if there's a semicolon between the gl_Position and the end of line
                std::string line = source.substr(pos, end_pos - pos);
                if (line.find(";") == std::string::npos) {
                    valid = false;
                }
            }
        }
        
        shader_compilation_status[shader] = valid;
    }
    
    void glGetShaderiv(GLuint shader, GLenum pname, int* params) {
        if (params && pname == GL_COMPILE_STATUS) {
            auto it = shader_compilation_status.find(shader);
            if (it != shader_compilation_status.end()) {
                *params = it->second ? GL_TRUE : GL_FALSE;
            } else {
                *params = GL_FALSE;  // Shader not found
            }
        }
    }
    
    void glDeleteShader(GLuint shader) {
        (void)shader;
    }
    
    GLuint glCreateProgram() {
        return dis(gen);  // Random program ID
    }
    
    void glAttachShader(GLuint program, GLuint shader) {
        (void)program; (void)shader;
    }
    
    void glLinkProgram(GLuint program) {
        (void)program;
    }
    
    void glGetProgramiv(GLuint program, GLenum pname, int* params) {
        (void)program; (void)pname;
        if (params) {
            *params = GL_TRUE;  // Always report linking success for now
        }
    }
    
    void glDeleteProgram(GLuint program) {
        (void)program;
    }
}

#endif // __ANDROID__