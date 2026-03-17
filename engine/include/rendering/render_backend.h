#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace d2::rendering {

// GL type aliases for the abstraction layer
using GLuint = uint32_t;
using GLenum = uint32_t;
using GLint = int32_t;
using GLsizei = int32_t;
using GLsizeiptr = ptrdiff_t;
using GLintptr = ptrdiff_t;
using GLboolean = uint8_t;

// GL constants used across the codebase
constexpr GLenum GL_NO_ERROR_VALUE = 0;
constexpr GLenum GL_INVALID_VALUE_VALUE = 0x0501;
constexpr GLenum GL_INVALID_ENUM_VALUE = 0x0500;
constexpr GLenum GL_INVALID_OPERATION_VALUE = 0x0502;
constexpr GLenum GL_OUT_OF_MEMORY_VALUE = 0x0505;

constexpr GLenum GL_VERTEX_SHADER_VALUE = 0x8B31;
constexpr GLenum GL_FRAGMENT_SHADER_VALUE = 0x8B30;
constexpr GLenum GL_COMPILE_STATUS_VALUE = 0x8B81;
constexpr GLenum GL_LINK_STATUS_VALUE = 0x8B82;
constexpr GLenum GL_FALSE_VALUE = 0;
constexpr GLenum GL_TRUE_VALUE = 1;

constexpr GLenum GL_ARRAY_BUFFER_VALUE = 0x8892;
constexpr GLenum GL_STATIC_DRAW_VALUE = 0x88E4;

constexpr GLenum GL_TEXTURE_2D_VALUE = 0x0DE1;
constexpr GLenum GL_RGBA_VALUE = 0x1908;
constexpr GLenum GL_UNSIGNED_BYTE_VALUE = 0x1401;
constexpr GLenum GL_TEXTURE_MIN_FILTER_VALUE = 0x2801;
constexpr GLenum GL_TEXTURE_MAG_FILTER_VALUE = 0x2800;
constexpr GLenum GL_TEXTURE_WRAP_S_VALUE = 0x2802;
constexpr GLenum GL_TEXTURE_WRAP_T_VALUE = 0x2803;
constexpr GLenum GL_LINEAR_VALUE = 0x2601;
constexpr GLenum GL_NEAREST_VALUE = 0x2600;
constexpr GLenum GL_CLAMP_TO_EDGE_VALUE = 0x812F;
constexpr GLenum GL_REPEAT_VALUE = 0x2901;

constexpr GLenum GL_TRIANGLES_VALUE = 0x0004;
constexpr GLenum GL_FLOAT_VALUE = 0x1406;
constexpr GLenum GL_BLEND_VALUE = 0x0BE2;
constexpr GLenum GL_SRC_ALPHA_VALUE = 0x0302;
constexpr GLenum GL_ONE_MINUS_SRC_ALPHA_VALUE = 0x0303;
constexpr GLenum GL_DEPTH_TEST_VALUE = 0x0B71;
constexpr GLenum GL_LEQUAL_VALUE = 0x0203;

// Abstract render backend interface
class IRenderBackend {
public:
    virtual ~IRenderBackend() = default;

    // Buffer operations
    virtual void genBuffers(GLsizei n, GLuint* buffers) = 0;
    virtual void bindBuffer(GLenum target, GLuint buffer) = 0;
    virtual void bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) = 0;
    virtual void bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) = 0;
    virtual void deleteBuffers(GLsizei n, const GLuint* buffers) = 0;

    // VAO operations
    virtual void genVertexArrays(GLsizei n, GLuint* arrays) = 0;
    virtual void bindVertexArray(GLuint array) = 0;
    virtual void deleteVertexArrays(GLsizei n, const GLuint* arrays) = 0;

    // Texture operations
    virtual void genTextures(GLsizei n, GLuint* textures) = 0;
    virtual void bindTexture(GLenum target, GLuint texture) = 0;
    virtual void texImage2D(GLenum target, GLint level, GLint internalformat,
                            GLsizei width, GLsizei height, GLint border,
                            GLenum format, GLenum type, const void* pixels) = 0;
    virtual void texParameteri(GLenum target, GLenum pname, GLint param) = 0;
    virtual void deleteTextures(GLsizei n, const GLuint* textures) = 0;

    // Shader operations
    virtual GLuint createShader(GLenum shaderType) = 0;
    virtual void shaderSource(GLuint shader, GLsizei count, const char* const* string, const GLint* length) = 0;
    virtual void compileShader(GLuint shader) = 0;
    virtual void getShaderiv(GLuint shader, GLenum pname, GLint* params) = 0;
    virtual void deleteShader(GLuint shader) = 0;

    // Program operations
    virtual GLuint createProgram() = 0;
    virtual void attachShader(GLuint program, GLuint shader) = 0;
    virtual void linkProgram(GLuint program) = 0;
    virtual void getProgramiv(GLuint program, GLenum pname, GLint* params) = 0;
    virtual void deleteProgram(GLuint program) = 0;
    virtual void useProgram(GLuint program) = 0;

    // Vertex attribute operations
    virtual void enableVertexAttribArray(GLuint index) = 0;
    virtual void vertexAttribPointer(GLuint index, GLint size, GLenum type,
                                     GLboolean normalized, GLsizei stride, const void* pointer) = 0;

    // Draw operations
    virtual void drawArrays(GLenum mode, GLint first, GLsizei count) = 0;
    virtual void drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) = 0;

    // State operations
    virtual void enable(GLenum cap) = 0;
    virtual void disable(GLenum cap) = 0;
    virtual void blendFunc(GLenum sfactor, GLenum dfactor) = 0;
    virtual void depthFunc(GLenum func) = 0;
    virtual void depthMask(GLboolean flag) = 0;

    // Error handling
    virtual GLenum getError() = 0;
};

} // namespace d2::rendering
