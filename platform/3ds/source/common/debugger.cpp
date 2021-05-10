#include "common/debugger.h"
#include <3ds.h>

using namespace love;

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
    if (this->IsInited())
        gdbHioDevExit();
}
