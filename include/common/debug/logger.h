/*
** logger.h
** @brief   : Logs shit when enabled
*/

#pragma once

#include "modules/thread/types/lock.h"

#include <stdarg.h>
#include <stdio.h>

class Logger
{
  public:
    static Logger& Instance()
    {
        static Logger instance;
        return instance;
    }

    void LogOutput(const char* func, size_t line, const char* format, ...) const;

    ~Logger();

  private:
    Logger();

    love::thread::MutexRef mutex;

    FILE* file;

    static constexpr const char* LOG_FORMAT = "%s:%zu:\n%s\n\n";
};

#if defined(__DEBUG__)
    #define LOG(format, ...) \
        Logger::Instance().LogOutput(__PRETTY_FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#endif