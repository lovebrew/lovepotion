#pragma once

#include "common/Data.hpp"
#include "common/Map.hpp"
#include "common/Module.hpp"

#include "modules/thread/Channel.hpp"
#include "modules/thread/LuaThread.hpp"
#include "modules/thread/Thread.hpp"

#include <map>
#include <string>

namespace love
{
    class ThreadModule : public Module
    {
      public:
        ThreadModule();

        virtual ~ThreadModule()
        {}

        LuaThread* newThread(const std::string& name, Data* data) const;

        Channel* newChannel() const;

        Channel* getChannel(const std::string& name);

      private:
        std::map<std::string, StrongRef<Channel>> channels;
        std::recursive_mutex mutex;
    };
} // namespace love
