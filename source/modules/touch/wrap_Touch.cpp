#include "modules/touch/wrap_Touch.hpp"

using namespace love;

#define instance() Module::getInstance<Touch>(Module::M_TOUCH)

namespace love
{
    int64_t luax_checktouchid(lua_State* L, int index)
    {
        if (!lua_islightuserdata(L, index))
            luax_typeerror(L, index, "touch id");

        return (int64_t)(intptr_t)lua_touserdata(L, index);
    }
} // namespace love

int Wrap_Touch::getTouches(lua_State* L)
{
    const auto& touches = instance()->getTouches();

    lua_createtable(L, touches.size(), 0);

    for (size_t index = 0; index < touches.size(); index++)
    {
        lua_pushlightuserdata(L, (void*)(intptr_t)touches[index].id);
        lua_rawseti(L, -2, (int)index + 1);
    }

    return 1;
}

int Wrap_Touch::getPosition(lua_State* L)
{
    int64_t id = luax_checktouchid(L, 1);

    Finger finger {};
    luax_catchexcept(L, [&] { finger = instance()->getTouch(id); });

    lua_pushnumber(L, finger.x);
    lua_pushnumber(L, finger.y);

    return 2;
}

int Wrap_Touch::getPressure(lua_State* L)
{
    int64_t id = luax_checktouchid(L, 1);

    Finger finger {};
    luax_catchexcept(L, [&] { finger = instance()->getTouch(id); });

    lua_pushnumber(L, finger.pressure);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getTouches",  Wrap_Touch::getTouches  },
    { "getPosition", Wrap_Touch::getPosition },
    { "getPressure", Wrap_Touch::getPressure }
};
// clang-format on

int Wrap_Touch::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&] { instance = new Touch(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "touch";
    module.type      = &Module::type;
    module.functions = functions;

    return luax_register_module(L, module);
}
