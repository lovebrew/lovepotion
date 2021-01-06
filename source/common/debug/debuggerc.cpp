#include "common/debug/debuggerc.h"
#include "common/debug/logger.h"

using namespace love::common;

Debugger::Debugger() : initialized(false),
                       count(0)
{}

bool Debugger::IsInited()
{
    return this->initialized;
}