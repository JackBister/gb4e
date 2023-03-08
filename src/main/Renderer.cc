#include "Renderer.hh"

#include <cassert>

#include "Common.hh"
#include "GbCpuState.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("GbRenderer");
namespace gb4e
{

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

    logger->Infof("GbRenderer constructor end");
}

void GbRenderer::SetFramebuffer(std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> const * framebuffer)
{
    this->framebuffer = framebuffer;
}

void GbRenderer::Draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(
        GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer->data());
}
};
