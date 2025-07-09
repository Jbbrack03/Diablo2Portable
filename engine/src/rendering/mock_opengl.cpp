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
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505

extern "C" {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dis(1000, 999999);
    
    // Shared OpenGL error state (accessible from texture_manager.cpp)
    extern GLenum current_error;
    
    // Static storage for shader compilation state
    static std::unordered_map<uint32_t, bool> shader_compilation_status;
    static std::unordered_map<uint32_t, std::string> shader_source_storage;
    
    // Static storage for VBO state tracking
    static std::unordered_map<uint32_t, size_t> vbo_sizes; // Track VBO sizes for validation
    static uint32_t currently_bound_buffer = 0; // Track currently bound buffer for size validation
    static constexpr size_t MAX_VBO_SIZE = 100 * 1024 * 1024; // 100MB max VBO size limit
    
    // Static storage for draw command tracking
    struct DrawArraysCall {
        uint32_t mode;
        int first;
        int count;
    };
    struct DrawElementsCall {
        uint32_t mode;
        int count;
        uint32_t type;
        uintptr_t indices;
    };
    static std::vector<DrawArraysCall> draw_arrays_calls;
    static std::vector<DrawElementsCall> draw_elements_calls;
    
    void glGenBuffers(GLsizei n, GLuint* buffers) {
        for (int i = 0; i < n; i++) {
            buffers[i] = dis(gen);  // Random buffer ID
        }
    }
    
    void glBindBuffer(GLenum target, GLuint buffer) {
        (void)target;
        // Track currently bound buffer for size validation
        currently_bound_buffer = buffer;
    }
    
    void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) {
        (void)target; (void)data; (void)usage;
        
        // Simulate real OpenGL memory limits - fail if size exceeds reasonable GPU memory
        if (size > static_cast<GLsizeiptr>(MAX_VBO_SIZE)) {
            current_error = GL_INVALID_VALUE; // Set OpenGL error for excessive memory allocation
            return;
        }
        
        // Track VBO size for later validation in glBufferSubData
        if (currently_bound_buffer != 0) {
            vbo_sizes[currently_bound_buffer] = static_cast<size_t>(size);
        }
    }
    
    void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data) {
        (void)target; (void)offset; (void)data;
        
        // Simulate real OpenGL validation - check if offset + size exceeds original buffer size
        if (currently_bound_buffer != 0) {
            auto it = vbo_sizes.find(currently_bound_buffer);
            if (it != vbo_sizes.end()) {
                size_t original_size = it->second;
                
                // Check if the update would exceed the original buffer size
                if (static_cast<size_t>(offset + size) > original_size) {
                    current_error = GL_INVALID_VALUE; // Set OpenGL error for buffer overflow
                    return;
                }
            }
        }
    }
    
    void glDeleteBuffers(GLsizei n, const GLuint* buffers) {
        // Clean up VBO size tracking when buffers are deleted
        for (int i = 0; i < n; i++) {
            if (buffers) {
                vbo_sizes.erase(buffers[i]);
                if (currently_bound_buffer == buffers[i]) {
                    currently_bound_buffer = 0;
                }
            }
        }
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
        GLenum error = current_error;
        current_error = GL_NO_ERROR; // Error is consumed after reading
        return error;
    }
    
    // Shader and draw call functions
    void glUseProgram(GLuint program) {
        (void)program;
    }
    
    void glDrawArrays(GLenum mode, int first, GLsizei count) {
        // Track the draw call
        draw_arrays_calls.push_back({static_cast<uint32_t>(mode), first, static_cast<int>(count)});
    }
    
    void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
        // Track the draw call
        draw_elements_calls.push_back({
            static_cast<uint32_t>(mode),
            static_cast<int>(count),
            static_cast<uint32_t>(type),
            reinterpret_cast<uintptr_t>(indices)
        });
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
    
    // Draw command tracking access functions
    void resetDrawCommandTracking() {
        draw_arrays_calls.clear();
        draw_elements_calls.clear();
    }
    
    size_t getDrawArraysCallCount() {
        return draw_arrays_calls.size();
    }
    
    size_t getDrawElementsCallCount() {
        return draw_elements_calls.size();
    }
    
    bool wasDrawArraysCalled() {
        return !draw_arrays_calls.empty();
    }
    
    bool wasDrawElementsCalled() {
        return !draw_elements_calls.empty();
    }
    
    const DrawArraysCall* getDrawArraysCalls() {
        return draw_arrays_calls.data();
    }
    
    const DrawElementsCall* getDrawElementsCalls() {
        return draw_elements_calls.data();
    }
}

#endif // __ANDROID__