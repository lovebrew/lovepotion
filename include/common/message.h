#pragma once

#include "common/variant.h"

#include <vector>

namespace love
{
    class Message : public Object
    {
      public:
        Message(const std::string& name, const std::vector<Variant>& args = {});

        virtual ~Message();

        static Message* FromLua(lua_State* L, int index);

        int ToLua(lua_State* L);

      private:
        const std::string name;
        const std::vector<Variant> args;
    };
} // namespace love
