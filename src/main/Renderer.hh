#pragma once

#include <array>

#include <gl/glew.h>

#include "Common.hh"

namespace gb4e
{

class Renderer
{
public:
    virtual void CopyFramebuffer(std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const & framebuffer) = 0;

    virtual void Draw() = 0;
};

class GbRenderer final : public Renderer
{
public:
    GbRenderer();

    void CopyFramebuffer(std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const & framebuffer) final override;

    void Draw() final override;

private:
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    GLuint texture;
    GLuint vertShader;
    GLuint fragShader;
    GLuint program;

    std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> framebuffer;
};

class NopRenderer final : public Renderer
{
public:
    void CopyFramebuffer(std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const & framebuffer) final override {}

    void Draw() final override {}
};
};