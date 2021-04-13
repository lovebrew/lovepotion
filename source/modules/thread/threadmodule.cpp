#include "modules/thread/threadmodule.h"

using namespace love;

LuaThread* ThreadModule::NewThread(const std::string& name, Data* data)
{
    return new LuaThread(name, data);
}

Channel* ThreadModule::NewChannel()
{
    return new Channel();
}

Channel* ThreadModule::GetChannel(const std::string& name)
{
    thread::Lock lock(this->namedChannelMutex);

    auto it = this->namedChannels.find(name);

    if (it != this->namedChannels.end())
        return it->second;

    Channel* channel = new Channel();
    namedChannels[name].Set(channel, Acquire::NORETAIN);

    return channel;
}