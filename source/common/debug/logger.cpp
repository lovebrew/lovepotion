#include "common/debug/logger.h"

using namespace love;

Logger::Logger()
{
    this->file = freopen("love.log", "w", stderr);
}

Logger::~Logger()
{
    fclose(this->file);
}

void Logger::LogOutput(const char* func, size_t line, const char* format, ...) const
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
