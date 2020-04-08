#include "common/runtime.h"
#include "threads/thread.h"

using namespace love::thread;

Threadable::Threadable(void * arg) : args(arg)
{}
