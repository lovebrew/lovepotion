#include <modules/graphics/wrap_graphics.hpp>

#include <modules/graphics_ext.hpp>

#include <objects/font/wrap_font.hpp>
#include <utilities/driver/renderer_ext.hpp>

using Renderer = love::Renderer<love::Console::Which>;

using namespace love;
#define instance() (Module::GetInstance<Graphics<Console::Which>>(Module::M_GRAPHICS))

#if !defined(__3DS__)
std::span<const luaL_Reg> Wrap_Graphics::extensions;
#endif

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

            newColor.value().r = luaL_checknumber(L, 1);
            newColor.value().g = luaL_checknumber(L, 2);
            newColor.value().b = luaL_checknumber(L, 3);
            newColor.value().a = luaL_optnumber(L, 4, 1.0);

            colors.push_back(newColor);

            lua_pop(L, 4);
        }
    }
    else if (argtype == LUA_TBOOLEAN)
        start = 2;
    else if (argtype != LUA_TNONE && argtype != LUA_TNIL)
    {
        color.value().r = luaL_checknumber(L, 1);
        color.value().g = luaL_checknumber(L, 2);
        color.value().b = luaL_checknumber(L, 3);
        color.value().a = luaL_optnumber(L, 4, 1.0);

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

        lua_pop(L, 4);
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

int Wrap_Graphics::GetBackgroundColor(lua_State* L)
{
    const auto color = instance()->GetBackgroundColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_Graphics::SetColor(lua_State* L)
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

        lua_pop(L, 4);
    }
    else if (lua_isnumber(L, 1))
    {
        color.r = luaL_checknumber(L, 1);
        color.g = luaL_checknumber(L, 2);
        color.b = luaL_checknumber(L, 3);
        color.a = luaL_optnumber(L, 4, 1.0f);
    }

    instance()->SetColor(color);

    return 0;
}

int Wrap_Graphics::GetColor(lua_State* L)
{
    const auto color = instance()->GetColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

// To do
int Wrap_Graphics::GetWidth(lua_State* L)
{
    const char* screenName = luaL_optstring(L, 1, "top");
    std::optional<Screen> screen;

    if (!(screen = ::Renderer::Instance().CheckScreen(screenName)))
        return luax::EnumError(L, "screen name", ::Renderer::Instance().GetScreens(), screenName);

    lua_pushinteger(L, instance()->GetWidth(*screen));

    return 1;
}

// To do
int Wrap_Graphics::GetHeight(lua_State* L)
{
    lua_pushinteger(L, instance()->GetHeight());

    return 1;
}

// To do
int Wrap_Graphics::GetDimensions(lua_State* L)
{
    const char* screenName = luaL_optstring(L, 1, "top");
    std::optional<Screen> screen;

    if (!(screen = ::Renderer::Instance().CheckScreen(screenName)))
        return luax::EnumError(L, "screen name", ::Renderer::Instance().GetScreens(), screenName);

    lua_pushinteger(L, instance()->GetWidth(*screen));
    lua_pushinteger(L, instance()->GetHeight());

    return 1;
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
    std::optional<Screen> screen;

    if (screen = ::Renderer::Instance().CheckScreen(name); !screen)
        return luax::EnumError(L, "screen name", ::Renderer::Instance().GetScreens(), name);

    instance()->SetActiveScreen(*screen);

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

int Wrap_Graphics::Print(lua_State* L)
{
    Font<>::ColoredStrings strings {};
    Wrap_Font::CheckColoredString(L, 1, strings);

    if (luax::IsType(L, 2, Font<>::type))
    {
        auto* font = Wrap_Font::CheckFont(L, 2);

        Wrap_Graphics::CheckStandardTransform(L, 3, [&](const Matrix4<Console::Which>& matrix) {
            luax::CatchException(L, [&]() { instance()->Print(strings, font, matrix); });
        });
    }
    else
    {
        Wrap_Graphics::CheckStandardTransform(L, 2, [&](const Matrix4<Console::Which>& matrix) {
            luax::CatchException(L, [&]() { instance()->Print(strings, matrix); });
        });
    }

    return 0;
}

int Wrap_Graphics::Printf(lua_State* L)
{
    Font<>::ColoredStrings strings {};
    Wrap_Font::CheckColoredString(L, 1, strings);

    Font<Console::Which>* font = nullptr;
    int start                  = 2;

    if (luax::IsType(L, start, Font<>::type))
    {
        font = Wrap_Font::CheckFont(L, start);
        start++;
    }

    Matrix4<Console::Which> matrix;

    int formatIndex = start + 2;

    /* todo check for Transform objects */
    // if (luax::IsType(L, start, love::Transform<love::Console::Which>::type))
    // {
    //     love::Transform<Console::Which>* tf = luax::ToType<Transform<Console::Which>>(L,
    //     start); func(tf->getMatrix());
    // }
    // else
    {
        float x  = luaL_optnumber(L, start + 0, 0.0);
        float y  = luaL_optnumber(L, start + 1, 0.0);
        float a  = luaL_optnumber(L, start + 4, 0.0);
        float sx = luaL_optnumber(L, start + 5, 1.0);
        float sy = luaL_optnumber(L, start + 6, sx);
        float ox = luaL_optnumber(L, start + 7, 0.0);
        float oy = luaL_optnumber(L, start + 8, 0.0);
        float kx = luaL_optnumber(L, start + 9, 0.0);
        float ky = luaL_optnumber(L, start + 10, 0.0);

        matrix = Matrix4<Console::Which>(x, y, a, sx, sy, ox, oy, kx, ky);
    }

    float wrap            = luaL_checknumber(L, formatIndex);
    const char* alignment = luaL_checkstring(L, formatIndex + 1);

    std::optional<Font<>::AlignMode> align;

    if (!(align = Font<>::alignModes.Find(alignment)))
        return luax::EnumError(L, "alignment", Font<>::alignModes.GetNames(), alignment);

    if (font != nullptr)
        luax::CatchException(L, [&]() { instance()->Printf(strings, font, wrap, *align, matrix); });
    else
        luax::CatchException(L, [&]() { instance()->Printf(strings, wrap, *align, matrix); });

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
    { "print",              Wrap_Graphics::Print              },
    { "printf",             Wrap_Graphics::Printf             },
    { "setActiveScreen",    Wrap_Graphics::SetActiveScreen    },
    { "setBackgroundColor", Wrap_Graphics::SetBackgroundColor },
    { "setColor",           Wrap_Graphics::SetColor           },
    { "getBackgroundColor", Wrap_Graphics::GetBackgroundColor },
    { "getColor",           Wrap_Graphics::GetColor           },
    { "getScreens",         Wrap_Graphics::GetScreens         },
    { "getWidth",           Wrap_Graphics::GetWidth           },
    { "getHeight",          Wrap_Graphics::GetHeight          },
    { "getDimensions",      Wrap_Graphics::GetDimensions      },
    { "reset",              Wrap_Graphics::Reset              }
};

static constexpr lua_CFunction types[] =
{
    Wrap_Font::Register,
    nullptr
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
    module.instance          = instance;
    module.name              = "graphics";
    module.functions         = functions;
    module.extendedFunctions = extensions;
    module.type              = &Module::type;
    module.types             = types;

    return luax::RegisterModule(L, module);
}
