#pragma once

#include <memory>
#include <vector>

#include "../LogAppender.hh"

class CompositeAppender : public LogAppender
{
public:
    CompositeAppender(std::vector<std::shared_ptr<LogAppender>> appenders);

protected:
    virtual void AppendImpl(LogMessage const & message) final override;

private:
    std::vector<std::shared_ptr<LogAppender>> appenders;
};
