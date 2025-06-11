#pragma once

#include <string>
#include <cstdint>

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

private:
    uint32_t next_shader_id_ = 1;
};

} // namespace d2::rendering