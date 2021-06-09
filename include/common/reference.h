#pragma once

struct lua_State;

namespace love
{
    class Reference
    {
      public:
        Reference();

        Reference(lua_State* L);

        virtual ~Reference();

        void Ref(lua_State* L);

        void UnRef();

        void Push(lua_State* L);

      private:
        lua_State* pinnedState;
        int index;
    };
} // namespace love
