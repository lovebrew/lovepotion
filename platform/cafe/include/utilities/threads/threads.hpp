#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

namespace love
{
    using mutex       = std::mutex;
    using conditional = std::condition_variable;
    using thread      = std::thread;
} // namespace love
