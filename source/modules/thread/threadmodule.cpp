#include <modules/thread/threadmodule.hpp>
#include <utilities/threads/threads.hpp>

using namespace love;

LuaThread* ThreadModule::NewThread(const std::string& name, Data* code) const
{
    return new LuaThread(name, code);
}

Channel* ThreadModule::NewChannel() const
{
    return new Channel();
}

Channel* ThreadModule::GetChannel(const std::string& name)
{
    std::unique_lock lock(this->mutex);

    auto iterator = this->namedChannels.find(name);

    if (iterator != this->namedChannels.end())
        return iterator->second;

    Channel* channel = new Channel();
    this->namedChannels[name].Set(channel, Acquire::NORETAIN);

    return channel;
}
