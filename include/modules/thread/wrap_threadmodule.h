#include "common/luax.h"

namespace Wrap_ThreadModule
{
    int NewThread(lua_State* L);

    int NewChannel(lua_State* L);

    int GetChannel(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_ThreadModule
