#include "modules/keyboard/wrap_Keyboard.hpp"

using namespace love;

#define instance() (Module::getInstance<Keyboard>(Module::M_KEYBOARD))

int Wrap_Keyboard::setKeyRepeat(lua_State* L)
{
    instance()->setKeyRepeat(luax_checkboolean(L, 1));

    return 0;
}

int Wrap_Keyboard::hasKeyRepeat(lua_State* L)
{
    luax_pushboolean(L, instance()->hasKeyRepeat());

    return 1;
}

int Wrap_Keyboard::isDown(lua_State* L)
{
    luax_pushboolean(L, false);

    return 1;
}

int Wrap_Keyboard::isScancodeDown(lua_State* L)
{
    luax_pushboolean(L, false);

    return 1;
}

int Wrap_Keyboard::getScancodeFromKey(lua_State* L)
{
    lua_pushstring(L, "unknown");

    return 1;
}

int Wrap_Keyboard::getKeyFromScancode(lua_State* L)
{
    lua_pushstring(L, "unknown");

    return 1;
}

int Wrap_Keyboard::isModifierActive(lua_State* L)
{
    luax_pushboolean(L, false);

    return 1;
}

int Wrap_Keyboard::setTextInput(lua_State* L)
{
    // clang-format off
    luax_checktablefields<Keyboard::KeyboardOption>(L, 1, "keyboard option", Keyboard::getConstant);
    // clang-format on

    Keyboard::KeyboardOptions options {
        type : Keyboard::TYPE_NORMAL,
        password : false,
        hint : "",
        maxLength : Keyboard::DEFAULT_INPUT_LENGTH
    };

    lua_getfield(L, 1, Keyboard::getConstant(Keyboard::OPTION_TYPE));
    if (!lua_isnoneornil(L, -1))
    {
        const char* string = luaL_checkstring(L, -1);

        Keyboard::KeyboardType type;
        if (!Keyboard::getConstant(string, type))
            luax_enumerror(L, "keyboard type", string);

        options.type = type;
    }
    lua_pop(L, 1);

    lua_getfield(L, 1, Keyboard::getConstant(Keyboard::OPTION_PASSCODE));
    if (!lua_isnoneornil(L, -1))
        options.password = luax_checkboolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, Keyboard::getConstant(Keyboard::OPTION_HINT));
    if (!lua_isnoneornil(L, -1))
        options.hint = luax_checkstring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, Keyboard::getConstant(Keyboard::OPTION_MAX_LENGTH));
    if (!lua_isnoneornil(L, -1))
        options.maxLength = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    instance()->setTextInput(options);

    return 0;
}

int Wrap_Keyboard::hasTextInput(lua_State* L)
{
    luax_pushboolean(L, instance()->hasTextInput());

    return 1;
}

int Wrap_Keyboard::hasScreenKeyboard(lua_State* L)
{
    luax_pushboolean(L, instance()->hasScreenKeyboard());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setKeyRepeat",       Wrap_Keyboard::setKeyRepeat       },
    { "hasKeyRepeat",       Wrap_Keyboard::hasKeyRepeat       },
    { "isDown",             Wrap_Keyboard::isDown             },
    { "isScancodeDown",     Wrap_Keyboard::isScancodeDown     },
    { "getScancodeFromKey", Wrap_Keyboard::getScancodeFromKey },
    { "getKeyFromScancode", Wrap_Keyboard::getKeyFromScancode },
    { "isModifierActive",   Wrap_Keyboard::isModifierActive   },
    { "setTextInput",       Wrap_Keyboard::setTextInput       },
    { "hasTextInput",       Wrap_Keyboard::hasTextInput       },
    { "hasScreenKeyboard",  Wrap_Keyboard::hasScreenKeyboard  }
};
// clang-format on

int Wrap_Keyboard::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Keyboard(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "keyboard";
    module.type      = &Module::type;
    module.functions = functions;

    return luax_register_module(L, module);
}
