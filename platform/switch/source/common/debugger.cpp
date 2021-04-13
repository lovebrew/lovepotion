#include "common/debugger.h"
#include <switch.h>

using namespace love;

Debugger::Debugger() : sockfd(-1)
{
    if (this->IsInited())
        return;

    this->sockfd      = nxlinkStdio();
    this->initialized = (this->sockfd != -1);
}

Debugger::~Debugger()
{
    if (this->sockfd != -1)
        close(this->sockfd);
}