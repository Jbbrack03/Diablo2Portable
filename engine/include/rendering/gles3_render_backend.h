#pragma once

#ifdef __ANDROID__

#include "rendering/render_backend.h"

namespace d2::rendering {

// Thin wrapper around real OpenGL ES 3.0 calls.
// Only compiled on Android.
class GLES3RenderBackend : public IRenderBackend {
public:
    void genBuffers(GLsizei n, GLuint* buffers) override;
    void bindBuffer(GLenum target, GLuint buffer) override;
    void bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) override;
    void bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) override;
    void deleteBuffers(GLsizei n, const GLuint* buffers) override;

    void genVertexArrays(GLsizei n, GLuint* arrays) override;
    void bindVertexArray(GLuint array) override;
    void deleteVertexArrays(GLsizei n, const GLuint* arrays) override;

    void genTextures(GLsizei n, GLuint* textures) override;
    void bindTexture(GLenum target, GLuint texture) override;
    void texImage2D(GLenum target, GLint level, GLint internalformat,
                    GLsizei width, GLsizei height, GLint border,
                    GLenum format, GLenum type, const void* pixels) override;
    void texParameteri(GLenum target, GLenum pname, GLint param) override;
    void deleteTextures(GLsizei n, const GLuint* textures) override;

    GLuint createShader(GLenum shaderType) override;
    void shaderSource(GLuint shader, GLsizei count, const char* const* string, const GLint* length) override;
    void compileShader(GLuint shader) override;
    void getShaderiv(GLuint shader, GLenum pname, GLint* params) override;
    void deleteShader(GLuint shader) override;

    GLuint createProgram() override;
    void attachShader(GLuint program, GLuint shader) override;
    void linkProgram(GLuint program) override;
    void getProgramiv(GLuint program, GLenum pname, GLint* params) override;
    void deleteProgram(GLuint program) override;
    void useProgram(GLuint program) override;

    void enableVertexAttribArray(GLuint index) override;
    void vertexAttribPointer(GLuint index, GLint size, GLenum type,
                             GLboolean normalized, GLsizei stride, const void* pointer) override;

    void drawArrays(GLenum mode, GLint first, GLsizei count) override;
    void drawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) override;

    void enable(GLenum cap) override;
    void disable(GLenum cap) override;
    void blendFunc(GLenum sfactor, GLenum dfactor) override;
    void depthFunc(GLenum func) override;
    void depthMask(GLboolean flag) override;

    GLenum getError() override;
};

} // namespace d2::rendering

#endif // __ANDROID__
