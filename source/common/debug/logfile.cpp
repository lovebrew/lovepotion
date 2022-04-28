#include "common/debug/logfile.h"

using namespace love;

LogFile::LogFile()
{
    this->file = freopen("love.log", "w", stderr);
}

LogFile::~LogFile()
{
    fclose(this->file);
}

void LogFile::LogOutput(const char* func, size_t line, const char* format, ...) const
{
    if (!this->file)
        return;

    thread::Lock lock(this->mutex);

    va_list args;
    va_start(args, format);
    char buffer[255];

    vsnprintf(buffer, sizeof(buffer), format, args);
    fprintf(this->file, LOG_FORMAT, func, line, buffer);

    fflush(this->file);
}
