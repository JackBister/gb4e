#include "GbRenderer.hh"

#include <cassert>

#include "GbCpuState.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("GbRenderer");

int constexpr SCREEN_WIDTH = 160;
int constexpr SCREEN_HEIGHT = 144;

char const * const VERT_SHADER = R"!!!(
#version 150

in vec2 pos;
in vec2 texcoord;

out vec2 Texcoord;

void main() {
    Texcoord = texcoord;
	gl_Position = vec4(pos, -1.0, 1.0);
}
)!!!";

char const * const FRAG_SHADER = R"!!!(
#version 150

in vec2 Texcoord;

uniform sampler2D Texture;

out vec4 fragColor;

void main() {
    fragColor = texture(Texture, Texcoord);
}
)!!!";

// clang-format off
float const QUAD_VBO[] = {
    -1.0f,  1.0f, 0.0f, 1.0f, // Top left
     1.0f,  1.0f, 1.0f, 1.0f, // Top right
     1.0f, -1.0f, 1.0f, 0.0f, // Bottom right
    -1.0f, -1.0f, 0.0f, 0.0f, // Bottom left
};
// clang-format on

u32 const QUAD_EBO[] = {0, 1, 2, 2, 3, 0};

namespace gb4e
{

struct Pixel {
    u8 r, g, b, a;
};

Pixel pixels[SCREEN_HEIGHT][SCREEN_WIDTH];

GbRenderer::GbRenderer()
{
    logger->Infof("GbRenderer constructor");
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &VERT_SHADER, nullptr);
    glCompileShader(vertShader);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &FRAG_SHADER, nullptr);
    glCompileShader(fragShader);
    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VBO), QUAD_VBO, GL_STATIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_EBO), QUAD_EBO, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLint posLocation = glGetAttribLocation(program, "pos");
    GLint texcoordLocation = glGetAttribLocation(program, "texcoord");
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(texcoordLocation);
    assert(glGetError() == GL_NO_ERROR);
    logger->Infof("GbRenderer constructor end");
}

void GbRenderer::RenderFramebuffer(GbCpuState const * state)
{
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            auto & pixel = pixels[y][x];
            pixel.r = x;
            pixel.b = y;
            pixel.g = 0;
            pixel.a = 0xFF;
        }
    }

    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindVertexArray(vao);
    glUniform1i(glGetUniformLocation(program, "Texture"), 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
};
