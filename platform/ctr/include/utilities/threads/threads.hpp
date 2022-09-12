#pragma once

#include <thread/ctr_thread.h>

#include <mutex>

namespace love
{
    using mutex       = ctr::mutex;
    using conditional = ctr::condition_variable;
    using thread      = ctr::thread;
} // namespace love
