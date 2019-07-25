#include "common/runtime.h"

void Logger::Initialize()
{
    if (!DEBUG)
        return;

    file = freopen("love.log", "w", stderr);
}

bool Logger::IsEnabled()
{
    return DEBUG == 1;
}

FILE * Logger::GetFile()
{
    return file;
}

void Logger::Exit()
{
    if (!DEBUG)
        return;

    fclose(file);
}
