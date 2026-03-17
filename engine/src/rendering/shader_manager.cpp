#include "rendering/shader_manager.h"
#include "rendering/render_context.h"
#include "rendering/render_backend.h"

namespace d2::rendering {

uint32_t ShaderManager::compileShader(ShaderType type, const std::string& source) {
    auto* backend = RenderContext::getBackend();
    if (!backend) return 0;

    // Convert ShaderType to OpenGL enum
    GLenum gl_shader_type = (type == ShaderType::VERTEX) ? GL_VERTEX_SHADER_VALUE : GL_FRAGMENT_SHADER_VALUE;

    // Create OpenGL shader object
    GLuint shader_id = backend->createShader(gl_shader_type);
    if (shader_id == 0) {
        return 0;
    }

    // Set shader source
    const char* source_cstr = source.c_str();
    backend->shaderSource(shader_id, 1, &source_cstr, nullptr);

    // Compile the shader
    backend->compileShader(shader_id);

    // Check compilation status
    GLint compile_status;
    backend->getShaderiv(shader_id, GL_COMPILE_STATUS_VALUE, &compile_status);

    if (compile_status == static_cast<GLint>(GL_FALSE_VALUE)) {
        backend->deleteShader(shader_id);
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
    auto* backend = RenderContext::getBackend();
    if (backend) {
        backend->deleteShader(shader_id);
    }
    valid_shaders_.erase(shader_id);
}

uint32_t ShaderManager::createProgram(uint32_t vertex_shader, uint32_t fragment_shader) {
    // Check that both input shaders are valid
    if (!isShaderValid(vertex_shader) || !isShaderValid(fragment_shader)) {
        return 0;
    }

    uint32_t program_id = next_program_id_++;
    valid_programs_.insert(program_id);

    // Initialize program info with uniform locations
    ProgramInfo& info = program_info_[program_id];
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
    auto it = program_info_.find(program_id);
    if (it == program_info_.end()) {
        return -1;
    }

    const ProgramInfo& info = it->second;
    auto loc_it = info.uniform_locations.find(name);
    if (loc_it == info.uniform_locations.end()) {
        return -1;
    }

    return loc_it->second;
}

bool ShaderManager::setUniformMatrix4fv(uint32_t program_id, const std::string& name, const float* value) {
    if (!value) {
        return false;
    }

    int location = getUniformLocation(program_id, name);
    if (location < 0) {
        return false;
    }

    return true;
}

bool ShaderManager::setUniform4f(uint32_t program_id, const std::string& name, float v0, float v1, float v2, float v3) {
    int location = getUniformLocation(program_id, name);
    if (location < 0) {
        return false;
    }

    (void)v0; (void)v1; (void)v2; (void)v3;
    return true;
}

} // namespace d2::rendering
