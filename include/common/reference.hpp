#pragma once

struct lua_State;

namespace love
{
    class Reference
    {
      public:
        Reference();

        Reference(lua_State* L);

        ~Reference();

        void Create(lua_State* L);

        void UnReference();

        void Push(lua_State* L) const;

      private:
        lua_State* pinnedState;
        int index;
    };
} // namespace love