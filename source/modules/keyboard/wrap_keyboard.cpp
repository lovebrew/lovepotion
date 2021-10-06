#include "modules/keyboard/wrap_keyboard.h"

#include "modules/event/event.h"

using namespace love;

#define instance()     (Module::GetInstance<Keyboard>(Module::M_KEYBOARD))
#define EVENT_MODULE() (Module::GetInstance<love::Event>(Module::M_EVENT))

int Wrap_Keyboard::SetTextInput(lua_State* L)
{
    Keyboard::SwkbdOpt options;

    options.type       = Keyboard::KeyboardType::TYPE_NORMAL;
    options.hint       = "Enter String";
    options.isPassword = false;
    options.maxLength  = Keyboard::DEFAULT_INPUT_LENGTH;

    if (!lua_isnoneornil(L, 1))
    {
        Luax::CheckTableFields<Keyboard::KeyboardOption>(L, 1, "keyboard setting name",
                                                         common::Keyboard::GetConstant);

        /* keyboard type option */

        lua_getfield(L, 1, common::Keyboard::GetConstant(Keyboard::OPTION_TYPE));
        if (!lua_isnoneornil(L, -1))
        {
            const char* string = luaL_checkstring(L, -1);
            if (!Keyboard::GetConstant(string, options.type))
                return Luax::EnumError(L, "keyboard type", string);
        }
        lua_pop(L, 1);

        /* Hint string option */

        lua_getfield(L, 1, common::Keyboard::GetConstant(Keyboard::OPTION_HINT));
        if (!lua_isnoneornil(L, -1))
            options.hint = Luax::CheckString(L, -1);
        lua_pop(L, 1);

        /* passcode field option -- hides input */

        lua_getfield(L, 1, common::Keyboard::GetConstant(Keyboard::OPTION_PASSCODE));
        if (!lua_isnoneornil(L, -1))
            options.isPassword = Luax::CheckBoolean(L, -1);
        lua_pop(L, 1);

        /* max input length option */

        lua_getfield(L, 1, common::Keyboard::GetConstant(Keyboard::OPTION_MAX_LEN));
        if (!lua_isnoneornil(L, -1))
        {
            uint32_t length = luaL_checknumber(L, -1);
            options.maxLength =
                std::clamp(length, Keyboard::MINIMUM_INPUT_LENGTH, Keyboard::MAX_INPUT_LENGTH);
        }
        lua_pop(L, 1);
    }

    std::string text = instance()->SetTextInput(options);

    Luax::CatchException(L, [&]() {
        std::vector<Variant> args = { text };

        StrongReference<Message> message(new Message("textinput", args), Acquire::NORETAIN);

        EVENT_MODULE()->Push(message);
    });

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setTextInput", Wrap_Keyboard::SetTextInput },
    { 0,              0                           }
};
// clang-format on

int Wrap_Keyboard::Register(lua_State* L)
{
    Keyboard* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Keyboard(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "keyboard";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = nullptr;

    return Luax::RegisterModule(L, wrappedModule);
}
