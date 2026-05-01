#include "fatal.hpp"

#include <coreinit/debug.h>
#include <coreinit/exception.h>

#include <cstdio>
#include <ranges>
#include <string>

using CrashHandler = void (*)();

static OSContext crashContext {};
static uint32_t crashType {};

template<OSExceptionType Type>
static inline bool handleCrash(CrashHandler handler, OSContext* context)
{
    crashContext  = *context;
    crashType     = Type;
    context->srr0 = reinterpret_cast<uint32_t>(handler);
    return true;
}

static constexpr const char* getExceptionName(OSExceptionType t) noexcept
{
    switch (t)
    {
        case OS_EXCEPTION_TYPE_DSI:
            return "DSI";
        case OS_EXCEPTION_TYPE_ISI:
            return "ISI";
        case OS_EXCEPTION_TYPE_PROGRAM:
            return "PROGRAM";
        default:
            return "UNKNOWN";
    }
}

void handleFatalCrash()
{
    char buffer[0x400] {};
    size_t offset = 0;

    const char* name = getExceptionName((OSExceptionType)crashType);
    auto writeBuffer = [&buffer, &offset]<typename... Args>(const char* fmt, Args&&... args) {
        offset += std::snprintf(buffer + offset, sizeof(buffer) - offset, fmt, std::forward<Args>(args)...);
    };
    writeBuffer("A %s exception occurred:\n\n", name);

    const auto gprs = std::span(crashContext.gpr);
    for (auto [index, address] : std::views::enumerate(gprs))
        writeBuffer("r%-2zu: 0x%08X%s", index, address, (index % 5 == 4) ? "\n" : " ");

    writeBuffer("\n\n");

    writeBuffer("CR:    0x%08X ", crashContext.cr);
    writeBuffer("LR:   0x%08X ", crashContext.lr);
    writeBuffer("CTR: 0x%08X ", crashContext.ctr);
    writeBuffer("XER: 0x%08X ", crashContext.xer);
    writeBuffer("\n");
    writeBuffer("SRR0:  0x%08X ", crashContext.srr0);
    writeBuffer("SRR1: 0x%08X ", crashContext.srr1);
    writeBuffer("EX0: 0x%08X ", crashContext.dsisr);
    writeBuffer("EX1: 0x%08X", crashContext.dar);

    std::printf("%s\n", buffer);
    OSFatal(buffer);
}

int handleDSIFatal(OSContext* context)
{
    return handleCrash<OS_EXCEPTION_TYPE_DSI>(handleFatalCrash, context);
}

int handleISIFatal(OSContext* context)
{
    return handleCrash<OS_EXCEPTION_TYPE_ISI>(handleFatalCrash, context);
}

int handleProgramFatal(OSContext* context)
{
    return handleCrash<OS_EXCEPTION_TYPE_PROGRAM>(handleFatalCrash, context);
}
