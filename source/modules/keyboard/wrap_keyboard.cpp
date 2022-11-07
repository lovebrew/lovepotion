#include <modules/keyboard/wrap_keyboard.hpp>
#include <modules/keyboard_ext.hpp>

#include <common/message.hpp>
#include <common/strongreference.hpp>
#include <common/variant.hpp>

#include <modules/event/event.hpp>

using Keyboard = love::Keyboard<love::Console::Which>;
using namespace love;

#define instance() (Module::GetInstance<::Keyboard>(Module::M_KEYBOARD))
#define Event()    (Module::GetInstance<love::Event>(Module::M_EVENT))

int Wrap_Keyboard::SetTextInput(lua_State* L)
{
    ::Keyboard::KeyboardOptions options {};
    options.type       = ::Keyboard::KeyboardType::TYPE_NORMAL;
    options.hint       = "Enter String";
    options.isPassword = false;
    options.maxLength  = ::Keyboard::DEFAULT_INPUT_LENGTH;

    if (!lua_isnoneornil(L, 1))
    {

        luax::CheckTableFields<::Keyboard::KeyboardOptions>(
            L, 1, "keyboard setting name", [](const char* v) {
                std::optional<::Keyboard::KeyboardOption> value;
                return (::Keyboard::keyboardOptions.Find(v) != std::nullopt);
            });

        auto optionType = ::Keyboard::keyboardOptions.ReverseFind(::Keyboard::OPTION_TYPE);

        if (optionType)
        {
            lua_getfield(L, 1, *optionType);

            if (!lua_isnoneornil(L, -1))
            {
                const char* string = luaL_checkstring(L, -1);
                std::optional<uint8_t> type;

                if (!(type = ::Keyboard::keyboardTypes.Find(string)))
                    return luax::EnumError(L, "keyboard type", string);

                options.type = *type;
            }
        }
        lua_pop(L, 1);

        auto optionHint = ::Keyboard::keyboardOptions.ReverseFind(::Keyboard::OPTION_HINT);

        if (optionHint)
        {
            lua_getfield(L, 1, *optionHint);

            if (!lua_isnoneornil(L, -1))
                options.hint = luax::CheckString(L, -1);
        }
        lua_pop(L, 1);

        auto optionPasscode = ::Keyboard::keyboardOptions.ReverseFind(::Keyboard::OPTION_PASSCODE);

        if (optionPasscode)
        {
            lua_getfield(L, 1, *optionPasscode);

            if (!lua_isnoneornil(L, -1))
                options.isPassword = luax::CheckBoolean(L, -1);
        }
        lua_pop(L, 1);

        auto optionLength = ::Keyboard::keyboardOptions.ReverseFind(::Keyboard::OPTION_MAX_LENGTH);

        if (optionLength)
        {
            lua_getfield(L, 1, *optionLength);

            if (!lua_isnoneornil(L, -1))
            {
                uint32_t length   = luaL_checknumber(L, -1);
                options.maxLength = std::clamp<uint32_t>(length, ::Keyboard::MINIMUM_INPUT_LENGTH,
                                                         ::Keyboard::MAX_INPUT_LENGTH);
            }
        }
        lua_pop(L, 1);
    }

    instance()->SetTextInput(options);

    return 0;
}

int Wrap_Keyboard::HasTextInput(lua_State* L)
{
    luax::PushBoolean(L, instance()->HasTextInput());

    return 1;
}

int Wrap_Keyboard::HasScreenKeyboard(lua_State* L)
{
    luax::PushBoolean(L, instance()->HasScreenKeyboard());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setTextInput",      Wrap_Keyboard::SetTextInput      },
    { "hasTextInput",      Wrap_Keyboard::HasTextInput      },
    { "hasScreenKeyboard", Wrap_Keyboard::HasScreenKeyboard }
};
// clang-format on

int Wrap_Keyboard::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new ::Keyboard(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "keyboard";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = nullptr;

    return luax::RegisterModule(L, wrappedModule);
}
