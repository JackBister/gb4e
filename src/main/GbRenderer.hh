#pragma once

#include <gl/glew.h>

namespace gb4e
{
class GbCpuState;

class GbRenderer
{
public:
    GbRenderer();

    void RenderFramebuffer(GbCpuState const *);

private:
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    GLuint texture;
    GLuint vertShader;
    GLuint fragShader;
    GLuint program;
};
};