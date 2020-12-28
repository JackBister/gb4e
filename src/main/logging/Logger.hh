#pragma once

#include <cstdarg>
#include <memory>
#include <string>

class LogAppender;

class Logger
{
public:
    Logger(std::string name, std::shared_ptr<LogAppender> appender);

    static std::unique_ptr<Logger> Create(std::string const & name);

    void Tracef(char const * format, ...);
    void Infof(char const * format, ...);
    void Warnf(char const * format, ...);
    void Errorf(char const * format, ...);
    void Severef(char const * format, ...);

private:
    std::string Sprintf(char const * format, va_list args);

    std::shared_ptr<LogAppender> appender;
    std::string name;
};
