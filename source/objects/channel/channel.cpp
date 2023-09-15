#include <objects/channel/channel.hpp>

#include <modules/timer_ext.hpp>

#include <mutex>

using namespace love;
using Timer = love::Timer<Console::Which>;

Type Channel::type("Channel", &Object::type);

Channel::Channel() : sent(0), received(0)
{}

uint64_t Channel::_Push(const Variant& variant)
{
    this->queue.push(variant);
    this->condition.notify_all();

    return ++this->sent;
}

uint64_t Channel::Push(const Variant& variant)
{
    std::unique_lock lock(this->mutex);
    return this->_Push(variant);
}

bool Channel::Supply(const Variant& variant)
{
    std::unique_lock lock(this->mutex);
    uint64_t id = this->_Push(variant);

    while (this->received < id)
        this->condition.wait(lock);

    return true;
}

bool Channel::Supply(const Variant& variant, double timeout)
{
    std::unique_lock lock(this->mutex);
    uint64_t id = this->_Push(variant);

    while (timeout >= 0)
    {
        if (this->received >= id)
            return true;

        const auto start = ::Timer::GetTime();
        const auto ms    = std::chrono::milliseconds((int64_t)timeout * 1000);

        this->condition.wait_for(lock, ms);

        const auto stop = ::Timer::GetTime();

        timeout -= (stop - start);
    }

    return false;
}

bool Channel::_Pop(Variant* variant)
{
    if (this->queue.empty())
        return false;

    *variant = this->queue.front();
    this->queue.pop();

    this->received++;
    this->condition.notify_all();

    return true;
}

bool Channel::Pop(Variant* variant)
{
    std::unique_lock lock(this->mutex);
    return this->_Pop(variant);
}

bool Channel::Demand(Variant* variant)
{
    std::unique_lock lock(this->mutex);

    while (!this->_Pop(variant))
        this->condition.wait(lock);

    return true;
}

bool Channel::Demand(Variant* variant, double timeout)
{
    std::unique_lock lock(this->mutex);

    while (timeout >= 0)
    {
        if (this->_Pop(variant))
            return true;

        const auto start = ::Timer::GetTime();
        const auto ms    = std::chrono::milliseconds((int64_t)timeout * 1000);

        this->condition.wait_for(lock, ms);

        const auto stop = ::Timer::GetTime();

        timeout -= (stop - start);

        if (timeout == 0)
            return false;
    }

    return false;
}

bool Channel::Peek(Variant* variant)
{
    std::unique_lock lock(this->mutex);

    if (this->queue.empty())
        return false;

    *variant = this->queue.front();

    return true;
}

int Channel::GetCount()
{
    std::unique_lock lock(this->mutex);

    return (int)this->queue.size();
}

bool Channel::HasRead(uint64_t id)
{
    std::unique_lock lock(this->mutex);

    return this->received >= id;
}

void Channel::Clear()
{
    std::unique_lock lock(this->mutex);

    if (this->queue.empty())
        return;

    while (!this->queue.empty())
        this->queue.pop();

    this->received = this->sent;
    this->condition.notify_all();
}

void Channel::LockMutex()
{
    this->mutex.lock();
}

void Channel::UnlockMutex()
{
    this->mutex.unlock();
}
