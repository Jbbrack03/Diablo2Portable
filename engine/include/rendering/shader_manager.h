#pragma once

#include <string>
#include <cstdint>
#include <unordered_set>

namespace d2::rendering {

enum class ShaderType {
    VERTEX,
    FRAGMENT
};

class ShaderManager {
public:
    ShaderManager() = default;
    ~ShaderManager() = default;

    uint32_t compileShader(ShaderType type, const std::string& source);
    bool isShaderValid(uint32_t shader_id) const;
    void deleteShader(uint32_t shader_id);
    
    // Create a complete shader program from vertex and fragment shaders
    uint32_t createProgram(uint32_t vertex_shader, uint32_t fragment_shader);
    bool isProgramValid(uint32_t program_id) const;

private:
    uint32_t next_shader_id_ = 1;
    uint32_t next_program_id_ = 1000;
    std::unordered_set<uint32_t> valid_shaders_;
    std::unordered_set<uint32_t> valid_programs_;
};

} // namespace d2::rendering