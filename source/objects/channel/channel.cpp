#include "objects/channel/channel.h"

#include "modules/timer/timer.h"

using namespace love;

love::Type Channel::type("Channel", &Object::type);

Channel::Channel() : sent(0), received(0)
{}

Channel::~Channel()
{}

uint64_t Channel::_Push(const Variant& variant)
{
    this->queue.push(variant);
    condition->Broadcast();

    return ++sent;
}

uint64_t Channel::Push(const Variant& variant)
{
    thread::Lock lock(this->mutex);

    return this->_Push(variant);
}

bool Channel::Supply(const Variant& variant)
{
    thread::Lock lock(this->mutex);
    uint64_t id = this->_Push(variant);

    while (this->received < id)
    {
        this->condition->Wait(this->mutex);
    }

    return true;
}

bool Channel::Supply(const Variant& variant, double timeout)
{
    thread::Lock lock(this->mutex);
    uint64_t id = this->_Push(variant);

    while (timeout >= 0)
    {
        if (this->received >= id)
            return true;

        double start = love::Timer::GetTime();
        this->condition->Wait(this->mutex, timeout);
        double stop = love::Timer::GetTime();

        timeout -= (stop - start);
    }

    return false;
}

bool Channel::Pop(Variant* variant)
{
    thread::Lock lock(this->mutex);

    return this->_Pop(variant);
}

bool Channel::_Pop(Variant* variant)
{
    if (this->queue.empty())
        return false;

    *variant = this->queue.front();
    this->queue.pop();

    this->received++;
    this->condition->Broadcast();

    return true;
}

bool Channel::Demand(Variant* variant)
{
    thread::Lock lock(this->mutex);

    while (!this->_Pop(variant))
        this->condition->Wait(this->mutex);

    return true;
}

bool Channel::Demand(Variant* variant, double timeout)
{
    thread::Lock lock(this->mutex);

    while (timeout >= 0)
    {
        if (this->_Pop(variant))
            return true;

        double start = love::Timer::GetTime();
        this->condition->Wait(this->mutex, timeout * 1000);
        double stop = love::Timer::GetTime();

        timeout -= (stop - start);
    }

    return false;
}

bool Channel::Peek(Variant* variant)
{
    thread::Lock lock(this->mutex);

    if (this->queue.empty())
        return false;

    *variant = this->queue.front();

    return true;
}

int Channel::GetCount() const
{
    thread::Lock lock(this->mutex);

    return (int)this->queue.size();
}

bool Channel::HasRead(uint64_t id) const
{
    thread::Lock lock(this->mutex);

    return this->received >= id;
}

void Channel::Clear()
{
    thread::Lock lock(this->mutex);

    if (this->queue.empty())
        return;

    while (!this->queue.empty())
        this->queue.pop();

    this->received = this->sent;
    this->condition->Broadcast();
}

void Channel::LockMutex()
{
    this->mutex->Lock();
}

void Channel::UnlockMutex()
{
    this->mutex->Unlock();
}
