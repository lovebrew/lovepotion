#include "modules/thread/threads.hpp"

namespace love
{
    void Conditional::signal()
    {
        this->condition.notify_one();
    }

    void Conditional::broadcast()
    {
        this->condition.notify_all();
    }

    bool Conditional::wait(Mutex& mutex, int timeout)
    {
        Lock lock(mutex, std::adopt_lock);
        bool signaled = true;

        if (timeout < 0)
            this->condition.wait(lock);
        else
        {
            const auto duration = std::chrono::milliseconds(timeout);
            signaled            = this->condition.wait_for(lock, duration) == std::cv_status::no_timeout;
        }

        lock.release();
        return signaled;
    }
} // namespace love
