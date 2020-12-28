#pragma once

#include "../LogAppender.hh"

class StdoutLogAppender : public LogAppender
{
protected:
    virtual void AppendImpl(LogMessage const & message) final override;
};
