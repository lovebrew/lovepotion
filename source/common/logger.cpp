#include "common/runtime.h"

#define DEBUG 1

void Logger::Initialize()
{
    if (!IsEnabled())
        return;

    file = freopen("love.log", "w", stderr);
}

bool Logger::IsEnabled()
{
    return DEBUG;
}

FILE * Logger::GetFile()
{
    return file;
}

void Logger::Exit()
{
    if (!IsEnabled())
        return;

    fclose(file);
}
