/*
** logger.h
** @brief   : Logs shit when enabled
*/

#pragma once

class Logger
{
    public:
        static void Initialize(bool active = 0);

        static void Exit();

        static bool IsEnabled();

        static FILE * GetFile();

    private:
        static inline FILE * m_file = nullptr;
        static inline bool m_enabled = false;
};

/*
** {Function Name}:{Line}
** {Resolved printf stuff}
** {Newline}
** {Newline}
*/

#define LOG(format, ...) \
    if (Logger::IsEnabled()) \
        fprintf(Logger::GetFile(), "%s:%d:\n" format "\n\n", __PRETTY_FUNCTION__, __LINE__, ## __VA_ARGS__); \
        fflush(Logger::GetFile());
