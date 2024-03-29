#include <modules/touch/touch.hpp>
#include <modules/touch/wrap_touch.hpp>

using namespace love;

#define instance() (Module::GetInstance<Touch>(Module::M_TOUCH))

int64_t Wrap_Touch::CheckTouchID(lua_State* L, int index)
{
    if (!lua_islightuserdata(L, index))
        return luax::TypeError(L, index, "touch id");

    return (int64_t)(intptr_t)lua_touserdata(L, index);
}

int Wrap_Touch::GetTouches(lua_State* L)
{
    const auto& touches = instance()->GetTouches();

    lua_createtable(L, (int)touches.size(), 0);

    for (size_t index = 0; index < touches.size(); index++)
    {
        lua_pushlightuserdata(L, (void*)(intptr_t)touches[index].id);
        lua_rawseti(L, -2, (int)index + 1);
    }

    return 1;
}

int Wrap_Touch::GetPosition(lua_State* L)
{
    auto id = Wrap_Touch::CheckTouchID(L, 1);

    Finger touch {};
    luax::CatchException(L, [&]() { touch = instance()->GetTouch(id); });

    lua_pushnumber(L, touch.x);
    lua_pushnumber(L, touch.y);

    return 2;
}

int Wrap_Touch::GetPressure(lua_State* L)
{
    auto id = Wrap_Touch::CheckTouchID(L, 1);

    Finger touch {};
    luax::CatchException(L, [&]() { touch = instance()->GetTouch(id); });

    lua_pushnumber(L, touch.pressure);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getPosition", Wrap_Touch::GetPosition },
    { "getPressure", Wrap_Touch::GetPressure },
    { "getTouches",  Wrap_Touch::GetTouches  }
};
// clang-format on

int Wrap_Touch::Register(lua_State* L)
{
    Touch* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Touch(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "touch";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = nullptr;

    return luax::RegisterModule(L, wrappedModule);
}
