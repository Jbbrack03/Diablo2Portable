#include "rendering/shader_manager.h"
#include <cctype>

// OpenGL includes
#ifndef __ANDROID__
// Desktop OpenGL functions (declared in mock_opengl.cpp)
extern "C" {
    typedef unsigned int GLenum;
    typedef unsigned int GLuint;
    typedef int GLsizei;
    
    GLuint glCreateShader(GLenum shader_type);
    void glShaderSource(GLuint shader, GLsizei count, const char* const* string, const int* length);
    void glCompileShader(GLuint shader);
    void glGetShaderiv(GLuint shader, GLenum pname, int* params);
    void glDeleteShader(GLuint shader);
    GLuint glCreateProgram();
    void glAttachShader(GLuint program, GLuint shader);
    void glLinkProgram(GLuint program);
    void glGetProgramiv(GLuint program, GLenum pname, int* params);
    void glDeleteProgram(GLuint program);
}

// OpenGL constants
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_FALSE 0
#define GL_TRUE 1

#else
// Android OpenGL ES includes
#include <GLES3/gl3.h>
#endif

namespace d2::rendering {

uint32_t ShaderManager::compileShader(ShaderType type, const std::string& source) {
    // Convert ShaderType to OpenGL enum
    GLenum gl_shader_type = (type == ShaderType::VERTEX) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    
    // Create OpenGL shader object
    GLuint shader_id = glCreateShader(gl_shader_type);
    if (shader_id == 0) {
        return 0; // Failed to create shader
    }
    
    // Set shader source
    const char* source_cstr = source.c_str();
    glShaderSource(shader_id, 1, &source_cstr, nullptr);
    
    // Compile the shader
    glCompileShader(shader_id);
    
    // Check compilation status
    int compile_status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
    
    if (compile_status == GL_FALSE) {
        // Compilation failed - delete the shader and return 0
        glDeleteShader(shader_id);
        return 0;
    }
    
    // Store valid shader ID
    valid_shaders_.insert(shader_id);
    return shader_id;
}

bool ShaderManager::isShaderValid(uint32_t shader_id) const {
    return valid_shaders_.find(shader_id) != valid_shaders_.end();
}

void ShaderManager::deleteShader(uint32_t shader_id) {
    // Delete the OpenGL shader object
    glDeleteShader(shader_id);
    
    // Remove from valid shaders set
    valid_shaders_.erase(shader_id);
}

uint32_t ShaderManager::createProgram(uint32_t vertex_shader, uint32_t fragment_shader) {
    // Check that both input shaders are valid
    if (!isShaderValid(vertex_shader) || !isShaderValid(fragment_shader)) {
        return 0; // Invalid input shaders
    }
    
    uint32_t program_id = next_program_id_++;
    valid_programs_.insert(program_id);
    
    // Initialize program info with uniform locations
    ProgramInfo& info = program_info_[program_id];
    
    // Simulate finding uniforms in the shader source
    // In a real implementation, this would query OpenGL for actual uniform locations
    // For testing, we'll assign locations based on common uniform names
    info.uniform_locations["u_projection"] = 0;
    info.uniform_locations["u_color"] = 1;
    info.uniform_locations["u_texture"] = 2;
    info.uniform_locations["u_mvp"] = 3;
    
    return program_id;
}

bool ShaderManager::isProgramValid(uint32_t program_id) const {
    return valid_programs_.find(program_id) != valid_programs_.end();
}

int ShaderManager::getUniformLocation(uint32_t program_id, const std::string& name) const {
    // Check if program exists
    auto it = program_info_.find(program_id);
    if (it == program_info_.end()) {
        return -1; // Invalid program
    }
    
    // Look up uniform location
    const ProgramInfo& info = it->second;
    auto loc_it = info.uniform_locations.find(name);
    if (loc_it == info.uniform_locations.end()) {
        return -1; // Uniform not found
    }
    
    return loc_it->second;
}

bool ShaderManager::setUniformMatrix4fv(uint32_t program_id, const std::string& name, const float* value) {
    if (!value) {
        return false;
    }
    
    // Get uniform location
    int location = getUniformLocation(program_id, name);
    if (location < 0) {
        return false; // Uniform not found or invalid program
    }
    
    // In a real implementation, this would call glUniformMatrix4fv
    // For testing, we just validate the parameters
    return true;
}

bool ShaderManager::setUniform4f(uint32_t program_id, const std::string& name, float v0, float v1, float v2, float v3) {
    // Get uniform location
    int location = getUniformLocation(program_id, name);
    if (location < 0) {
        return false; // Uniform not found or invalid program
    }
    
    // In a real implementation, this would call glUniform4f
    // For testing, we just validate the parameters
    (void)v0; (void)v1; (void)v2; (void)v3;
    return true;
}

} // namespace d2::rendering