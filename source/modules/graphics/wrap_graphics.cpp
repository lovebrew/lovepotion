#include <modules/graphics/wrap_graphics.hpp>

#include <modules/graphics_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

using Renderer = love::Renderer<love::Console::Which>;

using namespace love;
#define instance() (Module::GetInstance<Graphics<Console::Which>>(Module::M_GRAPHICS))

static int checkGraphicsCreated(lua_State* L)
{
    if (!instance()->IsCreated())
        return luaL_error(L, "love.graphics cannot function without a window!");

    return 0;
}

int Wrap_Graphics::Reset(lua_State*)
{
    instance()->Reset();
    return 0;
}

int Wrap_Graphics::Clear(lua_State* L)
{
    OptionalColor color(Color {});
    std::vector<OptionalColor> colors;

    OptionalInt stencil(0);
    OptionalDouble depth(1.0);

    int argtype = lua_type(L, 1);
    int start   = -1;

    if (argtype == LUA_TTABLE)
    {
        int maxValues = lua_gettop(L);
        colors.reserve(maxValues);

        for (int index = 0; index < maxValues; index++)
        {
            argtype = lua_type(L, index + 1);
            if (argtype == LUA_TNUMBER || argtype == LUA_TBOOLEAN)
            {
                start = (index + 1);
                break;
            }
            else if (argtype == LUA_TNIL || argtype == LUA_TNONE)
            {
                colors.push_back(OptionalColor(Color {}));
                continue;
            }

            for (int colorIndex = 1; colorIndex <= 4; colorIndex++)
                lua_rawgeti(L, (index + 1), colorIndex);

            OptionalColor newColor {};
            auto& value = newColor.value();

            value.r = luaL_checknumber(L, -4);
            value.g = luaL_checknumber(L, -3);
            value.b = luaL_checknumber(L, -2);
            value.a = luaL_optnumber(L, -1, 1.0f);

            colors.push_back(newColor);

            lua_pop(L, 4);
        }
    }
    else if (argtype == LUA_TBOOLEAN)
        start = 2;
    else if (argtype != LUA_TNONE && argtype != LUA_TNIL)
    {
        auto& value = color.value();

        value.r = luaL_checknumber(L, 1);
        value.g = luaL_checknumber(L, 2);
        value.b = luaL_checknumber(L, 3);
        value.a = luaL_checknumber(L, 4);

        start = 5;
    }

    if (start >= 0)
    {
        argtype = lua_type(L, start);

        if (argtype == LUA_TBOOLEAN)
        {}
        else if (argtype == LUA_TNUMBER)
            stencil.value() = luaL_checkinteger(L, start);

        argtype = lua_type(L, start + 1);

        if (argtype == LUA_TBOOLEAN)
        {}
        else if (argtype == LUA_TNUMBER)
            depth.value() = luaL_checknumber(L, start + 1);
    }

    if (colors.empty())
        luax::CatchException(L, [&]() { instance()->Clear(color, stencil, depth); });
    else
        luax::CatchException(L, [&]() { instance()->Clear(colors, stencil, depth); });

    return 0;
}

int Wrap_Graphics::GetBackgroundColor(lua_State* L)
{
    const auto color = instance()->GetBackgroundColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_Graphics::SetBackgroundColor(lua_State* L)
{
    Color color {};

    if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 4; i++)
            lua_rawgeti(L, 1, i);

        color.r = luaL_checknumber(L, -4);
        color.g = luaL_checknumber(L, -3);
        color.b = luaL_checknumber(L, -2);
        color.a = luaL_optnumber(L, -1, 1.0f);
    }
    else if (lua_isnumber(L, 1))
    {
        color.r = luaL_checknumber(L, 1);
        color.g = luaL_checknumber(L, 2);
        color.b = luaL_checknumber(L, 3);
        color.a = luaL_optnumber(L, 4, 1.0f);
    }

    instance()->SetBackgroundColor(color);

    return 0;
}

int Wrap_Graphics::IsActive(lua_State* L)
{
    luax::PushBoolean(L, instance()->IsActive());

    return 1;
}

int Wrap_Graphics::IsCreated(lua_State* L)
{
    luax::PushBoolean(L, instance()->IsCreated());

    return 1;
}

int Wrap_Graphics::SetActiveScreen(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    Screen screen;

    ::Renderer::Instance().CheckScreen(L, name, &screen);
    instance()->SetActiveScreen(screen);

    return 0;
}

int Wrap_Graphics::GetScreens(lua_State* L)
{
    const auto screens = ::Renderer::Instance().GetScreens();

    lua_createtable(L, screens.size(), 0);

    for (size_t index = 0; index < screens.size(); index++)
    {
        luax::PushString(L, screens[index]);
        lua_rawseti(L, -2, (index + 1));
    }

    return 1;
}

int Wrap_Graphics::Origin(lua_State* L)
{
    instance()->Origin();

    return 0;
}

int Wrap_Graphics::Present(lua_State* L)
{
    luax::CatchException(L, [&]() { instance()->Present(); });

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clear",              Wrap_Graphics::Clear              },
    { "isActive",           Wrap_Graphics::IsActive           },
    { "isCreated",          Wrap_Graphics::IsCreated          },
    { "origin",             Wrap_Graphics::Origin             },
    { "present",            Wrap_Graphics::Present            },
    { "setActiveScreen",    Wrap_Graphics::SetActiveScreen    },
    { "setBackgroundColor", Wrap_Graphics::SetBackgroundColor },
    { "getBackgroundColor", Wrap_Graphics::GetBackgroundColor },
    { "getScreens",         Wrap_Graphics::GetScreens         },
    { "reset",              Wrap_Graphics::Reset              }
};

static constexpr lua_CFunction types[] =
{
    0
};
// clang-format on

int Wrap_Graphics::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Graphics<Console::Which>(); });
    else
        instance()->Retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "graphics";
    module.functions = functions;
    module.type      = &Module::type;
    module.types     = types;

    return luax::RegisterModule(L, module);
}
