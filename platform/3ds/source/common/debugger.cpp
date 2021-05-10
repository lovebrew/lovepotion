#include "common/debugger.h"
#include <3ds.h>

using namespace love;

Debugger::Debugger()
{}

bool Debugger::Initialize()
{
    if (this->initialized)
        return true;

    gdbHioDevInit();

    int success = gdbHioDevRedirectStdStreams(false, true, true);

    this->initialized = (success == 0);

    return this->initialized;
}

Debugger::~Debugger()
{
    if (this->initialized)
        gdbHioDevExit();
}
