#include "common/debugger.h"
#include <switch.h>

using namespace love;

Debugger::Debugger() : sockfd(-1)
{}

bool Debugger::Initialize()
{
    if (this->initialized)
        return true;

    this->sockfd = nxlinkStdio();

    this->initialized = (this->sockfd != -1);

    return this->initialized;
}

Debugger::~Debugger()
{
    if (this->sockfd != -1)
        close(this->sockfd);
}
