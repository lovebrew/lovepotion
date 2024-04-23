#include "common/screen.hpp"

#include "modules/graphics/wrap_Graphics.hpp"

using namespace love;

#define instance() (Module::getInstance<Graphics>(Module::M_GRAPHICS))

static int luax_checkgraphicscreated(lua_State* L)
{
    if (!instance()->isCreated())
        luaL_error(L, "love.graphics cannot function without a window!");

    return 0;
}

int Wrap_Graphics::reset(lua_State* L)
{
    instance()->reset();

    return 0;
}

int Wrap_Graphics::clear(lua_State* L)
{
    OptionalColor color(Color(0, 0, 0, 0));
    std::vector<OptionalColor> colors {};

    OptionalInt stencil(0);
    OptionalDouble depth(1.0);

    int argType = lua_type(L, 1);
    int start   = -1;

    if (argType == LUA_TTABLE)
    {
        int max = lua_gettop(L);
        colors.reserve(max);

        for (int index = 0; index < max; index++)
        {
            argType = lua_type(L, index + 1);

            if (argType == LUA_TNUMBER || argType == LUA_TBOOLEAN)
            {
                start = index + 1;
                break;
            }
            else if (argType == LUA_TNIL || argType == LUA_TNONE || luax_objlen(L, index + 1) == 0)
            {
                colors.push_back(OptionalColor());
                continue;
            }

            for (int j = 1; j <= 4; j++)
                lua_rawgeti(L, index + 1, j);

            OptionalColor _color {};
            _color.hasValue = true;

            _color.value.r = luaL_checknumber(L, -4);
            _color.value.g = luaL_checknumber(L, -3);
            _color.value.b = luaL_checknumber(L, -2);
            _color.value.a = luaL_checknumber(L, -1);

            colors.push_back(_color);

            lua_pop(L, 4);
        }
    }
    else if (argType == LUA_TBOOLEAN)
    {
        color.hasValue = luax_toboolean(L, 1);
        start          = 2;
    }
    else if (argType != LUA_TNONE && argType != LUA_TNIL)
    {
        color.hasValue = true;

        color.value.r = luaL_checknumber(L, 1);
        color.value.g = luaL_checknumber(L, 2);
        color.value.b = luaL_checknumber(L, 3);
        color.value.a = luaL_optnumber(L, 4, 1.0);

        start = 5;
    }

    if (start >= 0)
    {
        argType = lua_type(L, start);

        if (argType == LUA_TBOOLEAN)
            stencil.hasValue = luax_toboolean(L, start);
        else if (argType == LUA_TNUMBER)
            stencil.value = luaL_checkinteger(L, start);

        argType = lua_type(L, start + 1);

        if (argType == LUA_TBOOLEAN)
            depth.hasValue = luax_toboolean(L, start + 1);
        else if (argType == LUA_TNUMBER)
            depth.value = luaL_checknumber(L, start + 1);
    }

    if (colors.empty())
        luax_catchexcept(L, [&]() { instance()->clear(color, stencil, depth); });
    else
        luax_catchexcept(L, [&]() { instance()->clear(colors, stencil, depth); });

    return 0;
}

int Wrap_Graphics::present(lua_State* L)
{
    luax_catchexcept(L, [&]() { instance()->present(); });

    return 0;
}

int Wrap_Graphics::setColor(lua_State* L)
{
    Color color {};

    if (lua_istable(L, 1))
    {
        for (int index = 1; index <= 4; index++)
            lua_rawgeti(L, 1, index);

        color.r = luaL_checknumber(L, -4);
        color.g = luaL_checknumber(L, -3);
        color.b = luaL_checknumber(L, -2);
        color.a = luaL_optnumber(L, -1, 1.0);

        lua_pop(L, 4);
    }
    else
    {
        color.r = luaL_checknumber(L, 1);
        color.g = luaL_checknumber(L, 2);
        color.b = luaL_checknumber(L, 3);
        color.a = luaL_optnumber(L, 4, 1.0);
    }

    instance()->setColor(color);

    return 0;
}

int Wrap_Graphics::getColor(lua_State* L)
{
    Color color = instance()->getColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_Graphics::setBackgroundColor(lua_State* L)
{
    Color color {};

    if (lua_istable(L, 1))
    {
        for (int index = 1; index <= 4; index++)
            lua_rawgeti(L, 1, index);

        color.r = luaL_checknumber(L, -4);
        color.g = luaL_checknumber(L, -3);
        color.b = luaL_checknumber(L, -2);
        color.a = luaL_optnumber(L, -1, 1.0);

        lua_pop(L, 4);
    }
    else
    {
        color.r = luaL_checknumber(L, 1);
        color.g = luaL_checknumber(L, 2);
        color.b = luaL_checknumber(L, 3);
        color.a = luaL_optnumber(L, 4, 1.0);
    }

    instance()->setBackgroundColor(color);

    return 0;
}

int Wrap_Graphics::getBackgroundColor(lua_State* L)
{
    auto color = instance()->getBackgroundColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_Graphics::setColorMask(lua_State* L)
{
    ColorChannelMask mask {};

    if (lua_isnoneornil(L, 1))
        mask.r = mask.g = mask.b = mask.a = true;
    else if (lua_gettop(L) <= 1)
    {
        mask.r = mask.g = mask.b = mask.a = luax_checkboolean(L, 1);
    }
    else
    {
        mask.r = luax_toboolean(L, 1);
        mask.g = luax_toboolean(L, 2);
        mask.b = luax_toboolean(L, 3);
        mask.a = luax_toboolean(L, 4);
    }

    instance()->setColorMask(mask);

    return 0;
}

int Wrap_Graphics::getColorMask(lua_State* L)
{
    auto mask = instance()->getColorMask();

    lua_pushboolean(L, mask.r);
    lua_pushboolean(L, mask.g);
    lua_pushboolean(L, mask.b);
    lua_pushboolean(L, mask.a);

    return 4;
}

static BlendOperation luax_checkblendop(lua_State* L, int idx)
{
    auto blendOperation = BLENDOP_ADD;
    const char* str     = luaL_checkstring(L, idx);

    if (!love::getConstant(str, blendOperation))
        luax_enumerror(L, "blend operation", BlendOperations, str);

    return blendOperation;
}

static BlendFactor luax_checkblendfactor(lua_State* L, int idx)
{
    BlendFactor factor = BLENDFACTOR_ZERO;
    const char* str    = luaL_checkstring(L, idx);

    if (!love::getConstant(str, factor))
        luax_enumerror(L, "blend factor", BlendFactors, str);

    return factor;
}

static void luax_pushblendop(lua_State* L, BlendOperation operation)
{
    std::string_view name;
    if (!love::getConstant(operation, name))
        luaL_error(L, "unknown blend operation");

    luax_pushstring(L, name);
}

static void luax_pushblendfactor(lua_State* L, BlendFactor factor)
{
    std::string_view name;
    if (!getConstant(factor, name))
        luaL_error(L, "unknown blend factor");

    luax_pushstring(L, name);
}

int Wrap_Graphics::setBlendMode(lua_State* L)
{
    BlendMode mode;
    const char* name = luaL_checkstring(L, 1);

    if (!love::getConstant(name, mode))
        return luax_enumerror(L, "blend mode", BlendModes, name);

    auto alphaMode = BLENDALPHA_MULTIPLY;
    if (!lua_isnoneornil(L, 2))
    {
        const char* alphaName = luaL_checkstring(L, 2);
        if (!love::getConstant(alphaName, alphaMode))
            return luax_enumerror(L, "blend alpha mode", BlendAlphaModes, alphaName);
    }

    luax_catchexcept(L, [&]() { instance()->setBlendMode(mode, alphaMode); });

    return 0;
}

int Wrap_Graphics::getBlendMode(lua_State* L)
{
    std::string_view name;
    std::string_view alphaName;

    BlendAlpha alphaMode;
    const auto mode = instance()->getBlendMode(alphaMode);

    if (!love::getConstant(mode, name))
        luaL_error(L, "Unknown blend mode");

    if (!getConstant(alphaMode, alphaName))
        luaL_error(L, "Unknown blend alpha mode");

    luax_pushstring(L, name);
    luax_pushstring(L, alphaName);

    return 2;
}

int Wrap_Graphics::setBlendState(lua_State* L)
{
    BlendState state {};

    if (!lua_isnoneornil(L, 1))
    {
        state.enable = true;

        if (lua_gettop(L) >= 4)
        {
            state.operationRGB = luax_checkblendop(L, 1);
            state.operationA   = luax_checkblendop(L, 2);
            state.srcFactorRGB = luax_checkblendfactor(L, 3);
            state.srcFactorA   = luax_checkblendfactor(L, 4);
            state.dstFactorRGB = luax_checkblendfactor(L, 5);
            state.dstFactorA   = luax_checkblendfactor(L, 6);
        }
        else
        {
            state.operationRGB = state.operationA = luax_checkblendop(L, 1);
            state.srcFactorRGB = state.srcFactorA = luax_checkblendfactor(L, 2);
            state.dstFactorRGB = state.dstFactorA = luax_checkblendfactor(L, 3);
        }
    }

    luax_catchexcept(L, [&]() { instance()->setBlendState(state); });

    return 0;
}

int Wrap_Graphics::getBlendState(lua_State* L)
{
    const auto& state = instance()->getBlendState();

    if (state.enable)
    {
        luax_pushblendop(L, state.operationRGB);
        luax_pushblendop(L, state.operationA);
        luax_pushblendfactor(L, state.srcFactorRGB);
        luax_pushblendfactor(L, state.srcFactorA);
        luax_pushblendfactor(L, state.dstFactorRGB);
        luax_pushblendfactor(L, state.dstFactorA);
    }
    else
    {
        for (int i = 0; i < 6; i++)
            lua_pushnil(L);
    }

    return 6;
}

int Wrap_Graphics::setLineWidth(lua_State* L)
{
    float width = luaL_checknumber(L, 1);

    luax_catchexcept(L, [&]() { instance()->setLineWidth(width); });

    return 0;
}

int Wrap_Graphics::getLineWidth(lua_State* L)
{
    lua_pushnumber(L, instance()->getLineWidth());

    return 1;
}

int Wrap_Graphics::setLineJoin(lua_State* L)
{
    Graphics::LineJoin join;
    const char* str = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(str, join))
        return luax_enumerror(L, "line join", Graphics::LineJoins, str);

    instance()->setLineJoin(join);

    return 0;
}

int Wrap_Graphics::getLineJoin(lua_State* L)
{
    Graphics::LineJoin join = instance()->getLineJoin();
    std::string_view name;

    if (!Graphics::getConstant(join, name))
        luaL_error(L, "Unknown line join");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::setLineStyle(lua_State* L)
{
    Graphics::LineStyle style;
    const char* str = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(str, style))
        return luax_enumerror(L, "line style", Graphics::LineStyles, str);

    instance()->setLineStyle(style);

    return 0;
}

int Wrap_Graphics::getLineStyle(lua_State* L)
{
    Graphics::LineStyle style = instance()->getLineStyle();
    std::string_view name;

    if (!Graphics::getConstant(style, name))
        luaL_error(L, "Unknown line style");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::setPointSize(lua_State* L)
{
    float size = luaL_checknumber(L, 1);
    instance()->setPointSize(size);

    return 0;
}

int Wrap_Graphics::getPointSize(lua_State* L)
{
    lua_pushnumber(L, instance()->getPointSize());

    return 1;
}

int Wrap_Graphics::setMeshCullMode(lua_State* L)
{
    CullMode mode;
    const char* str = luaL_checkstring(L, 1);

    if (!love::getConstant(str, mode))
        return luax_enumerror(L, "cull mode", love::CullModes, str);

    instance()->setMeshCullMode(mode);

    return 0;
}

int Wrap_Graphics::getMeshCullMode(lua_State* L)
{
    auto cullMode = instance()->getMeshCullMode();
    std::string_view name;

    if (!love::getConstant(cullMode, name))
        luaL_error(L, "Unknown cull mode");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::setFrontFaceWinding(lua_State* L)
{
    Winding winding;
    const char* str = luaL_checkstring(L, 1);

    if (!love::getConstant(str, winding))
        return luax_enumerror(L, "winding", love::WindingModes, str);

    instance()->setFrontFaceWinding(winding);

    return 0;
}

int Wrap_Graphics::getFrontFaceWinding(lua_State* L)
{
    auto winding = instance()->getFrontFaceWinding();
    std::string_view name;

    if (!love::getConstant(winding, name))
        luaL_error(L, "Unknown winding");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::isCreated(lua_State* L)
{
    if (!instance()->isCreated())
        return luaL_error(L, "love.graphics cannot function without a window!");

    return 0;
}

int Wrap_Graphics::isActive(lua_State* L)
{
    lua_pushboolean(L, instance()->isActive());

    return 1;
}

int Wrap_Graphics::isGammaCorrect(lua_State* L)
{
    lua_pushboolean(L, love::isGammaCorrect());

    return 1;
}

int Wrap_Graphics::getRendererInfo(lua_State* L)
{
    auto info = instance()->getRendererInfo();

    lua_pushstring(L, info.name.c_str());
    lua_pushstring(L, info.version.c_str());
    lua_pushstring(L, info.vendor.c_str());
    lua_pushstring(L, info.device.c_str());

    return 4;
}

int Wrap_Graphics::getDPIScale(lua_State* L)
{
    lua_pushnumber(L, instance()->getScreenDPIScale());

    return 1;
}

int Wrap_Graphics::setScissor(lua_State* L)
{
    int argc = lua_gettop(L);

    if (argc == 0 ||
        (argc == 4 && lua_isnil(L, 1) && lua_isnil(L, 2) && lua_isnil(L, 3) && lua_isnil(L, 4)))
    {
        instance()->setScissor();
        return 0;
    }

    Rect scissor {};
    scissor.x = luaL_checkinteger(L, 1);
    scissor.y = luaL_checkinteger(L, 2);
    scissor.w = luaL_checkinteger(L, 3);
    scissor.h = luaL_checkinteger(L, 4);

    if (scissor.w < 0 || scissor.h < 0)
        return luaL_error(L, "Can't set scissor with negative width and/or height.");

    instance()->setScissor(scissor);

    return 0;
}

int Wrap_Graphics::intersectScissor(lua_State* L)
{
    Rect scissor {};
    scissor.x = luaL_checkinteger(L, 1);
    scissor.y = luaL_checkinteger(L, 2);
    scissor.w = luaL_checkinteger(L, 3);
    scissor.h = luaL_checkinteger(L, 4);

    if (scissor.w < 0 || scissor.h < 0)
        return luaL_error(L, "Can't set scissor with negative width and/or height.");

    instance()->intersectScissor(scissor);

    return 0;
}

int Wrap_Graphics::getScissor(lua_State* L)
{
    Rect scissor {};
    if (!instance()->getScissor(scissor))
        return 0;

    lua_pushinteger(L, scissor.x);
    lua_pushinteger(L, scissor.y);
    lua_pushinteger(L, scissor.w);
    lua_pushinteger(L, scissor.h);

    return 4;
}

int Wrap_Graphics::getStackDepth(lua_State* L)
{
    lua_pushinteger(L, instance()->getStackDepth());

    return 1;
}

int Wrap_Graphics::push(lua_State* L)
{
    auto stackType   = Graphics::STACK_TRANSFORM;
    const char* name = lua_isnoneornil(L, 1) ? nullptr : luaL_checkstring(L, 1);

    if (name && !Graphics::getConstant(name, stackType))
        return luax_enumerror(L, "graphics stack type", Graphics::StackTypes, name);

    luax_catchexcept(L, [&]() { instance()->push(stackType); });

    /* TODO: check transform type */

    return 0;
}

int Wrap_Graphics::pop(lua_State* L)
{
    luax_catchexcept(L, [&]() { instance()->pop(); });

    return 0;
}

int Wrap_Graphics::rotate(lua_State* L)
{
    float angle = luaL_checknumber(L, 1);
    instance()->rotate(angle);

    return 0;
}

int Wrap_Graphics::scale(lua_State* L)
{
    float sx = luaL_checknumber(L, 1);
    float sy = luaL_optnumber(L, 2, sx);

    instance()->scale(sx, sy);

    return 0;
}

int Wrap_Graphics::translate(lua_State* L)
{
    float dx = luaL_checknumber(L, 1);
    float dy = luaL_checknumber(L, 2);

    instance()->translate(dx, dy);

    return 0;
}

int Wrap_Graphics::shear(lua_State* L)
{
    float kx = luaL_checknumber(L, 1);
    float ky = luaL_checknumber(L, 2);

    instance()->shear(kx, ky);

    return 0;
}

int Wrap_Graphics::origin(lua_State* L)
{
    instance()->origin();

    return 0;
}

int Wrap_Graphics::applyTransform(lua_State* L)
{
    // clang-format off
    luax_checkstandardtransform(L, 1, [&](const Matrix4& transform) {
        instance()->applyTransform(transform);
    });
    // clang-format on

    return 0;
}

int Wrap_Graphics::replaceTransform(lua_State* L)
{
    // clang-format off
    luax_checkstandardtransform(L, 1, [&](const Matrix4& transform) {
        instance()->replaceTransform(transform);
    });
    // clang-format on

    return 0;
}

int Wrap_Graphics::transformPoint(lua_State* L)
{
    Vector2 point {};
    point.x = luaL_checknumber(L, 1);
    point.y = luaL_checknumber(L, 2);

    point = instance()->transformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Graphics::inverseTransformPoint(lua_State* L)
{
    Vector2 point {};
    point.x = luaL_checknumber(L, 1);
    point.y = luaL_checknumber(L, 2);

    point = instance()->inverseTransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

// Homebrew Stuff™

int Wrap_Graphics::getScreens(lua_State* L)
{
    auto screens = love::getScreenInfo();
    lua_createtable(L, screens.size(), 0);

    for (size_t i = 0; i < screens.size(); i++)
    {
        luax_pushstring(L, screens[i].name);
        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

int Wrap_Graphics::setActiveScreen(lua_State* L)
{
    std::string name = luax_checkstring(L, 1);
    Screen screen    = love::getScreenId(name);

    love::currentScreen = screen;

    return 0;
}

// clang-format off
#if defined(__3DS__)
int Wrap_Graphics::is3D(lua_State* L)
{
    lua_pushboolean(L, instance()->is3D());

    return 1;
}

int Wrap_Graphics::set3D(lua_State* L)
{
    bool enable = luax_toboolean(L, 1);
    instance()->set3D(enable);

    return 0;
}

int Wrap_Graphics::isWide(lua_State* L)
{
    lua_pushboolean(L, instance()->isWide());

    return 1;
}

int Wrap_Graphics::setWide(lua_State* L)
{
    bool enable = luax_toboolean(L, 1);
    instance()->setWide(enable);

    return 0;
}

int Wrap_Graphics::getDepth(lua_State* L)
{
    lua_pushnumber(L, instance()->getDepth());

    return 1;
}

static constexpr std::array<luaL_Reg, 5> platformFunctions =
{{
    { "is3D",     Wrap_Graphics::is3D     },
    { "set3D",    Wrap_Graphics::set3D    },
    { "isWide",   Wrap_Graphics::isWide   },
    { "setWide",  Wrap_Graphics::setWide  },
    { "getDepth", Wrap_Graphics::getDepth }
}};
#else
static constexpr std::span<const luaL_Reg> platformFunctions = {};
#endif

static constexpr luaL_Reg functions[] =
{
    { "reset",                  Wrap_Graphics::reset                 },
    { "clear",                  Wrap_Graphics::clear                 },
    { "present",                Wrap_Graphics::present               },
    { "setColor",               Wrap_Graphics::setColor              },
    { "getColor",               Wrap_Graphics::getColor              },
    { "setBackgroundColor",     Wrap_Graphics::setBackgroundColor    },
    { "getBackgroundColor",     Wrap_Graphics::getBackgroundColor    },
    { "setColorMask",           Wrap_Graphics::setColorMask          },
    { "getColorMask",           Wrap_Graphics::getColorMask          },
    { "setBlendMode",           Wrap_Graphics::setBlendMode          },
    { "getBlendMode",           Wrap_Graphics::getBlendMode          },
    { "setBlendState",          Wrap_Graphics::setBlendState         },
    { "getBlendState",          Wrap_Graphics::getBlendState         },
    { "setLineWidth",           Wrap_Graphics::setLineWidth          },
    { "getLineWidth",           Wrap_Graphics::getLineWidth          },
    { "setLineJoin",            Wrap_Graphics::setLineJoin           },
    { "getLineJoin",            Wrap_Graphics::getLineJoin           },
    { "setLineStyle",           Wrap_Graphics::setLineStyle          },
    { "getLineStyle",           Wrap_Graphics::getLineStyle          },
    { "setPointSize",           Wrap_Graphics::setPointSize          },
    { "getPointSize",           Wrap_Graphics::getPointSize          },
    { "setMeshCullMode",        Wrap_Graphics::setMeshCullMode       },
    { "getMeshCullMode",        Wrap_Graphics::getMeshCullMode       },
    { "setFrontFaceWinding",    Wrap_Graphics::setFrontFaceWinding   },
    { "getFrontFaceWinding",    Wrap_Graphics::getFrontFaceWinding   },
    { "isCreated",              Wrap_Graphics::isCreated             },
    { "isActive",               Wrap_Graphics::isActive              },
    { "isGammaCorrect",         Wrap_Graphics::isGammaCorrect        },
    { "getRendererInfo",        Wrap_Graphics::getRendererInfo       },
    { "getDPIScale",            Wrap_Graphics::getDPIScale           },
    { "setScissor",             Wrap_Graphics::setScissor            },
    { "intersectScissor",       Wrap_Graphics::intersectScissor      },
    { "getScissor",             Wrap_Graphics::getScissor            },
    { "getStackDepth",          Wrap_Graphics::getStackDepth         },
    { "push",                   Wrap_Graphics::push                  },
    { "pop",                    Wrap_Graphics::pop                   },
    { "rotate",                 Wrap_Graphics::rotate                },
    { "scale",                  Wrap_Graphics::scale                 },
    { "translate",              Wrap_Graphics::translate             },
    { "shear",                  Wrap_Graphics::shear                 },
    { "origin",                 Wrap_Graphics::origin                },
    { "applyTransform",         Wrap_Graphics::applyTransform        },
    { "replaceTransform",       Wrap_Graphics::replaceTransform      },
    { "transformPoint",         Wrap_Graphics::transformPoint        },
    { "inverseTransformPoint",  Wrap_Graphics::inverseTransformPoint },

    { "setActiveScreen", Wrap_Graphics::setActiveScreen },
    { "getScreens",      Wrap_Graphics::getScreens      },
};
// clang-format on

int Wrap_Graphics::open(lua_State* L)
{
    auto* instance = instance();
    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Graphics(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance          = instance;
    module.name              = "graphics";
    module.type              = &Module::type;
    module.functions         = functions;
    module.platformFunctions = platformFunctions;

    return luax_register_module(L, module);
}
