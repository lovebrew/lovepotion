#include "common/debugger.h"
#include <3ds.h>

using namespace love;

Debugger::Debugger()
{
    if (Debugger::IsInited())
        return;

    gdbHioDevInit();

    int success = gdbHioDevRedirectStdStreams(false, true, true);

    this->initialized = (success == 0);
}

Debugger::~Debugger()
{
    if (Debugger::IsInited())
        gdbHioDevExit();
}