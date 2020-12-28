#include "LoggerFactory.hh"

#include "LogAppender.hh"
#include "Logger.hh"

#include "appenders/CompositeAppender.hh"
#include "appenders/StdoutLogAppender.hh"
#include "ui/Console.hh"

LoggerFactory::LoggerFactory(std::shared_ptr<LogAppender> appender) : appender(appender) {}

LoggerFactory * LoggerFactory::GetInstance()
{
    static LoggerFactory * singleton = nullptr;
    if (singleton == nullptr) {
        auto consoleAppender = gb4e::ui::GetConsoleAppender();
        // TODO: Find a better place to configure this
        auto stdOutAppender = std::make_shared<StdoutLogAppender>();
        // stdOutAppender->SetMinimumLevel("GbCpu", LogLevel::INFO);
        std::vector<std::shared_ptr<LogAppender>> appenders = {stdOutAppender, consoleAppender};
        auto compositeAppender = std::make_shared<CompositeAppender>(appenders);
        singleton = new LoggerFactory(compositeAppender);
    }
    return singleton;
}

std::unique_ptr<Logger> LoggerFactory::CreateLogger(std::string name)
{
    return std::make_unique<Logger>(name, appender);
}
