#include "common/runtime.h"

void Logger::Initialize(bool active)
{
    if (!active)
        return;

    m_file = freopen("love.log", "w", stderr);

    if (!m_file)
        return;

    m_enabled = active;
}

bool Logger::IsEnabled()
{
    return m_enabled;
}

FILE * Logger::GetFile()
{
    return m_file;
}

void Logger::Exit()
{
    if (!IsEnabled())
        return;

    fclose(m_file);
}
