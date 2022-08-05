#pragma once

#include <array>

#include <gl/glew.h>

#include "Common.hh"

namespace gb4e
{

class Renderer
{
public:
    virtual void SetFramebuffer(std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const * framebuffer) = 0;

    virtual void Draw() = 0;
};

class GbRenderer final : public Renderer
{
public:
    GbRenderer();

    void SetFramebuffer(std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const * framebuffer) final override;

    void Draw() final override;

private:
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    GLuint texture;
    GLuint vertShader;
    GLuint fragShader;
    GLuint program;

    std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const * framebuffer;
};

class FakeRenderer final : public Renderer
{

public:
    void SetFramebuffer(std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const * fb) final override
    {
        this->framebuffer = fb;
    }

    void Draw() final override {}

    std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const * GetFramebuffer() const { return framebuffer; }

private:
    std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const * framebuffer;
};

}