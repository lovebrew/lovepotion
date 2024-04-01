#include "modules/thread/Channel.hpp"
#include "modules/timer/Timer.hpp"

using std::chrono_literals::operator""ms;

namespace love
{
    Type Channel::type("Channel", &Object::type);

    Channel::Channel() : sent(0), received(0)
    {}

    uint64_t Channel::push(const Variant& value)
    {
        std::unique_lock lock(this->mutex);

        this->queue.push(value);
        this->condvar.notify_all();

        return ++this->sent;
    }

    bool Channel::supply(const Variant& value)
    {
        std::unique_lock lock(this->mutex);
        uint64_t id = this->push(value);

        while (this->received < id)
            this->condvar.wait(lock);

        return true;
    }

    bool Channel::supply(const Variant& value, double timeout)
    {
        std::unique_lock lock(this->mutex);
        uint64_t id = this->push(value);

        while (timeout >= 0)
        {
            if (this->received >= id)
                return true;

            double start = Timer::getTime();
            this->condvar.wait_for(lock, timeout * 1000ms);
            double stop = Timer::getTime();

            timeout -= (stop - start);
        }

        return false;
    }

    bool Channel::pop(Variant* value)
    {
        std::unique_lock lock(this->mutex);

        if (this->queue.empty())
            return false;

        *value = this->queue.front();
        this->queue.pop();

        this->received++;
        this->condvar.notify_all();

        return true;
    }

    bool Channel::demand(Variant* value)
    {
        std::unique_lock lock(this->mutex);

        while (!this->pop(value))
            this->condvar.wait(lock);

        return true;
    }

    bool Channel::demand(Variant* value, double timeout)
    {
        std::unique_lock lock(this->mutex);

        while (timeout >= 0)
        {
            if (this->pop(value))
                return true;

            double start = Timer::getTime();
            this->condvar.wait_for(lock, timeout * 1000ms);
            double stop = Timer::getTime();

            timeout -= (stop - start);
        }

        return false;
    }

    bool Channel::peek(Variant* value)
    {
        std::unique_lock lock(this->mutex);

        if (this->queue.empty())
            return false;

        *value = this->queue.front();

        return true;
    }

    int Channel::getCount()
    {
        std::unique_lock lock(this->mutex);

        return (int)this->queue.size();
    }

    bool Channel::hasRead(uint64_t id)
    {
        std::unique_lock lock(this->mutex);

        return this->received >= id;
    }

    void Channel::clear()
    {
        std::unique_lock lock(this->mutex);

        while (!this->queue.empty())
            this->queue.pop();

        this->received = this->sent;
        this->condvar.notify_all();
    }

    void Channel::lockMutex()
    {
        this->mutex.lock();
    }

    void Channel::unlockMutex()
    {
        this->mutex.unlock();
    }
} // namespace love
