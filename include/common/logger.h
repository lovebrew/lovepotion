/*
** logger.h
** @brief   : Logs shit when DEBUG is set to 1
*/

class Logger
{
    public:
        static void Initialize();

        static void Exit();

        static bool IsEnabled();

        static FILE * GetFile();

    private:
        static inline FILE * file = nullptr;
};

/*
** {Function Name}:{Line}
** {Resolved printf stuff}
** {Newline}
** {Newline}
*/

#define LOG(format, ...) \
    if (Logger::IsEnabled()) \
        fprintf(Logger::GetFile(), "%s:%d:\n" format "\n\n", __PRETTY_FUNCTION__, __LINE__, ## __VA_ARGS__)
