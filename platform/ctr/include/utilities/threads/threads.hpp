#pragma once

#include <thread/ctr_thread.h>

namespace love
{
    using mutex       = ctr::mutex;
    using conditional = ctr::condition_variable;
    using thread      = ctr::thread;
} // namespace love
