#include "common/runtime.h"
#include "modules/keyboard/wrap_keyboard.h"

#include "modules/event/event.h"

using namespace love;

#define instance() (Module::GetInstance<Keyboard>(Module::M_KEYBOARD))
#define LOVE_EVENT() (Module::GetInstance<love::Event>(Module::M_EVENT))

int Wrap_Keyboard::SetTextInput(lua_State * L)
{
    Keyboard::SwkbdOpt options;

    if (!lua_istable(L, 1) && !lua_isnoneornil(L, 1))
        luaL_argerror(L, 1, "table or nil expected");
    else if (lua_istable(L, 1))
        lua_settop(L, 1);
    else if (lua_isnoneornil(L, 1))
        lua_newtable(L);

    lua_getfield(L, 1, "type");
    lua_getfield(L, 1, "password");
    lua_getfield(L, 1, "hint");
    lua_getfield(L, 1, "length");

    options.type = luaL_optstring(L, -4, "basic");
    options.isPassword = lua_toboolean(L, -3);
    options.hint = luaL_optstring(L, -2, "Enter Text");

    size_t length = luaL_optnumber(L, -1, KEYBOARD_STRING_DEFAULT_LEN);
    options.maxLength = std::clamp(length, (size_t)1, (size_t)KEYBOARD_STRING_MAX_LEN);

    std::string text = instance()->SetTextInput(options);

    Luax::GetLove(L, "textinput");
    lua_pushstring(L, text.c_str());

    lua_call(L, 1, 0);

    return 0;
}

int Wrap_Keyboard::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "setTextInput", SetTextInput },
        { 0,              0            }
    };

    Keyboard * instance = instance();
    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Keyboard(); });
    else
        instance->Retain();

    WrappedModule module;

    module.instance = instance;
    module.name = "keyboard";
    module.functions = reg;
    module.type = &Module::type;
    module.types = nullptr;

    return Luax::RegisterModule(L, module);
}
