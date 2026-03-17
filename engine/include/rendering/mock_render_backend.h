#pragma once

#include "rendering/render_backend.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <random>

namespace d2::rendering {

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

class MockRenderBackend : public IRenderBackend {
public:
    MockRenderBackend();

    // Buffer operations
    void genBuffers(GLsizei n, GLuint* buffers) override;
    void bindBuffer(GLenum target, GLuint buffer) override;
    void bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) override;
    void bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) override;
    void deleteBuffers(GLsizei n, const GLuint* buffers) override;

    // VAO operations
    void genVertexArrays(GLsizei n, GLuint* arrays) override;
    void bindVertexArray(GLuint array) override;
    void deleteVertexArrays(GLsizei n, const GLuint* arrays) override;

    // Texture operations
    void genTextures(GLsizei n, GLuint* textures) override;
    void bindTexture(GLenum target, GLuint texture) override;
    void texImage2D(GLenum target, GLint level, GLint internalformat,
                    GLsizei width, GLsizei height, GLint border,
                    GLenum format, GLenum type, const void* pixels) override;
    void texParameteri(GLenum target, GLenum pname, GLint param) override;
    void deleteTextures(GLsizei n, const GLuint* textures) override;

    // Shader operations
    GLuint createShader(GLenum shaderType) override;
    void shaderSource(GLuint shader, GLsizei count, const char* const* string, const GLint* length) override;
    void compileShader(GLuint shader) override;
    void getShaderiv(GLuint shader, GLenum pname, GLint* params) override;
    void deleteShader(GLuint shader) override;

    // Program operations
    GLuint createProgram() override;
    void attachShader(GLuint program, GLuint shader) override;
    void linkProgram(GLuint program) override;
    void getProgramiv(GLuint program, GLenum pname, GLint* params) override;
    void deleteProgram(GLuint program) override;
    void useProgram(GLuint program) override;

    // Vertex attribute operations
    void enableVertexAttribArray(GLuint index) override;
    void vertexAttribPointer(GLuint index, GLint size, GLenum type,
                             GLboolean normalized, GLsizei stride, const void* pointer) override;

    // Draw operations
    void drawArrays(GLenum mode, GLint first, GLsizei count) override;
    void drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) override;

    // State operations
    void enable(GLenum cap) override;
    void disable(GLenum cap) override;
    void blendFunc(GLenum sfactor, GLenum dfactor) override;
    void depthFunc(GLenum func) override;
    void depthMask(GLboolean flag) override;

    // Error handling
    GLenum getError() override;

    // Test inspection API (preserved from mock_opengl.cpp)
    void resetDrawCommandTracking();
    size_t getDrawArraysCallCount() const;
    size_t getDrawElementsCallCount() const;
    bool wasDrawArraysCalled() const;
    bool wasDrawElementsCalled() const;
    const std::vector<DrawArraysCall>& getDrawArraysCalls() const;
    const std::vector<DrawElementsCall>& getDrawElementsCalls() const;

private:
    // RNG for buffer/VAO/shader IDs
    std::mt19937 gen_;

    // Error state
    GLenum currentError_ = GL_NO_ERROR_VALUE;

    // Shader state
    std::unordered_map<uint32_t, bool> shaderCompilationStatus_;
    std::unordered_map<uint32_t, std::string> shaderSourceStorage_;

    // VBO state
    std::unordered_map<uint32_t, size_t> vboSizes_;
    uint32_t currentlyBoundBuffer_ = 0;
    static constexpr size_t MAX_VBO_SIZE = 100 * 1024 * 1024;

    // Texture state
    uint32_t nextTextureId_ = 1;

    // Draw command tracking
    std::vector<DrawArraysCall> drawArraysCalls_;
    std::vector<DrawElementsCall> drawElementsCalls_;
};

} // namespace d2::rendering
