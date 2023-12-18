#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

namespace love
{
    using mutex           = std::mutex;
    using conditional     = std::condition_variable;
    using thread          = std::thread;
    using recursive_mutex = std::recursive_mutex;
    using condvar_any     = std::condition_variable_any;
} // namespace love
