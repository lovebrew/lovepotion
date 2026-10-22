#include "modules/thread/Channel.hpp"
#include "modules/timer/Timer.hpp"

namespace love
{
    Type Channel::type("Channel", &Object::type);

    Channel::Channel() : sent(0), received(0)
    {}

    uint64_t Channel::push(const Variant& value)
    {
        Lock lock(this->mutex);

        this->queue.push(value);
        this->condvar.broadcast();

        return ++this->sent;
    }

    bool Channel::supply(const Variant& value)
    {
        Lock lock(this->mutex);
        uint64_t id = this->push(value);

        while (this->received < id)
            this->condvar.wait(this->mutex);

        return true;
    }

    bool Channel::supply(const Variant& value, double timeout)
    {
        Lock lock(this->mutex);
        uint64_t id = this->push(value);

        while (timeout >= 0)
        {
            if (this->received >= id)
                return true;

            double start = Timer::getTime();
            this->condvar.wait(this->mutex, timeout * 1000);
            double stop = Timer::getTime();

            timeout -= (stop - start);
        }

        return false;
    }

    bool Channel::pop(Variant* value)
    {
        Lock lock(this->mutex);

        if (this->queue.empty())
            return false;

        *value = this->queue.front();
        this->queue.pop();

        this->received++;
        this->condvar.broadcast();

        return true;
    }

    bool Channel::demand(Variant* value)
    {
        Lock lock(this->mutex);

        while (!this->pop(value))
            this->condvar.wait(this->mutex);

        return true;
    }

    bool Channel::demand(Variant* value, double timeout)
    {
        Lock lock(this->mutex);

        while (timeout >= 0)
        {
            if (this->pop(value))
                return true;

            double start = Timer::getTime();
            this->condvar.wait(this->mutex, timeout * 1000);
            double stop = Timer::getTime();

            timeout -= (stop - start);
        }

        return false;
    }

    bool Channel::peek(Variant* value)
    {
        Lock lock(this->mutex);

        if (this->queue.empty())
            return false;

        *value = this->queue.front();

        return true;
    }

    int Channel::getCount()
    {
        Lock lock(this->mutex);
        return (int)this->queue.size();
    }

    bool Channel::hasRead(uint64_t id)
    {
        Lock lock(this->mutex);
        return this->received >= id;
    }

    void Channel::clear()
    {
        Lock lock(this->mutex);

        while (!this->queue.empty())
            this->queue.pop();

        this->received = this->sent;
        this->condvar.broadcast();
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
