#pragma once

#include "modules/thread/types/threadable.h"

#include "common/data.h"
#include <common/variant.h>

#include <vector>

namespace love
{
    class LuaThread : public Threadable
    {
      public:
        static love::Type type;

        LuaThread(const std::string& name, love::Data* code);
        ~LuaThread();

        void ThreadFunction();

        const std::string& GetError() const;

        bool Start(const std::vector<Variant>& args);

      private:
        void OnError();

        StrongReference<love::Data> code;

        std::string name;
        std::string error;

        std::vector<Variant> args;
    };
} // namespace love
