#include "common/runtime.h"

using namespace love;

Logger::Logger()
{}

Logger::~Logger()
{}

void Logger::LogOutput(const char * func, size_t line, const char * format, ...) const
{
    thread::Lock lock(this->mutex);

    va_list args;
    va_start(args, format);
    char buffer[255];

    vsnprintf(buffer, sizeof(buffer), format, args);
    fprintf(stderr, LOG_FORMAT, func, line, buffer);

    fflush(stderr);
}
