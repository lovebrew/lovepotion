#include "modules/thread/ThreadModule.hpp"

namespace love
{
    ThreadModule::ThreadModule() : Module(M_THREAD, "love.thread")
    {}

    LuaThread* ThreadModule::newThread(const std::string& name, Data* data) const
    {
        return new LuaThread(name, data);
    }

    Channel* ThreadModule::newChannel() const
    {
        return new Channel();
    }

    Channel* ThreadModule::getChannel(const std::string& name)
    {
        std::unique_lock lock(this->mutex);

        auto it = this->channels.find(name);
        if (it != this->channels.end())
            return it->second;

        Channel* channel = new Channel();
        this->channels[name].set(channel, Acquire::NO_RETAIN);

        return channel;
    }
} // namespace love
