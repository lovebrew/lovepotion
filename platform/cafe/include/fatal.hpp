#pragma once

#include <coreinit/context.h>
#include <cstdint>

extern "C"
{
    int handleDSIFatal(OSContext* context);

    int handleISIFatal(OSContext* context);

    int handleProgramFatal(OSContext* context);

    void handleFatalCrash();
}
