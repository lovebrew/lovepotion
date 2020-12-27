#include "common/debugger.h"
#include <switch.h>

using namespace love;

Debugger::Debugger()
{
    if (Debugger::IsInited())
        return;

    this->sockfd = nxlinkStdioForDebug();

    this->initialized = (this->sockfd != -1);
}

Debugger::~Debugger()
{}