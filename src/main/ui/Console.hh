#pragma once

#include <memory>

class LogAppender;

namespace gb4e::ui
{
std::shared_ptr<LogAppender> GetConsoleAppender();
void DrawConsole();
};
