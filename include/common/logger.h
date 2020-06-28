/*
** logger.h
** @brief   : Logs shit when enabled
*/

#pragma once

#include "modules/thread/types/lock.h"

namespace Logger
{
    void Initialize();

    void Exit();

    void LogOutput(const char * func, size_t line, const char * format, ...);

    inline FILE * m_file = nullptr;
    inline bool m_enabled = false;
    static love::thread::MutexRef m_mutex;
}

#define LOG(format, ...) \
    Logger::LogOutput(__PRETTY_FUNCTION__, __LINE__, format, ## __VA_ARGS__)
