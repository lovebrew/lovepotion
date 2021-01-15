#include "common/debug/debuggerc.h"

using namespace love::common;

Debugger::Debugger() : initialized(false)
{}

bool Debugger::IsInited()
{
    return this->initialized;
}