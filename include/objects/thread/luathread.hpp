#pragma once

#include <common/data.hpp>
#include <common/strongreference.hpp>
#include <common/variant.hpp>

#include <utilities/threads/threadable.hpp>

#include <vector>

namespace love
{
    class LuaThread : public Threadable
    {
      public:
        static Type type;

        LuaThread(const std::string& name, Data* code);

        virtual ~LuaThread()
        {}

        void ThreadFunction();

        const std::string& GetError() const;

        bool HasError() const
        {
            return this->hasError;
        }

        bool Start(const std::vector<Variant>& args);

      private:
        void OnError();

        StrongReference<Data> code;
        std::string name;
        std::string error;
        bool hasError;

        std::vector<Variant> args;
    };
} // namespace love
