#include "CompositeAppender.hh"

CompositeAppender::CompositeAppender(std::vector<std::shared_ptr<LogAppender>> appenders) : appenders(appenders) {}

void CompositeAppender::AppendImpl(LogMessage const & message)
{
    for (auto appender : appenders) {
        appender->Append(message);
    }
}
