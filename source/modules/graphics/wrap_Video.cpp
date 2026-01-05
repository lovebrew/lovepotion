#include "modules/graphics/wrap_Video.hpp"

using namespace love;

static constexpr char wrap_video_lua[] = {
#include "modules/graphics/wrap_Video.lua"
};

int Wrap_Video::getStream(lua_State* L)
{
    auto* self = luax_checkvideo(L, 1);
    luax_pushtype(L, self->getStream());

    return 1;
}

int Wrap_Video::getSource(lua_State* L)
{
    auto* self   = luax_checkvideo(L, 1);
    auto* source = self->getSource();

    if (source)
        luax_pushtype(L, self->getSource());
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Video::setSource(lua_State* L)
{
    auto* self = luax_checkvideo(L, 1);

    if (lua_isnoneornil(L, 2))
        self->setSource(nullptr);
    else
    {
        auto* source = luax_checktype<SourceBase>(L, 2);
        self->setSource(source);
    }

    return 0;
}

int Wrap_Video::getWidth(lua_State* L)
{
    auto* self = luax_checkvideo(L, 1);
    lua_pushnumber(L, self->getWidth());

    return 1;
}

int Wrap_Video::getHeight(lua_State* L)
{
    auto* self = luax_checkvideo(L, 1);
    lua_pushnumber(L, self->getHeight());

    return 1;
}

int Wrap_Video::getDimensions(lua_State* L)
{
    auto* self = luax_checkvideo(L, 1);

    lua_pushnumber(L, self->getWidth());
    lua_pushnumber(L, self->getHeight());

    return 2;
}

int Wrap_Video::getPixelWidth(lua_State* L)
{
    auto* self = luax_checkvideo(L, 1);
    lua_pushnumber(L, self->getPixelWidth());

    return 1;
}

int Wrap_Video::getPixelHeight(lua_State* L)
{
    auto* self = luax_checkvideo(L, 1);
    lua_pushnumber(L, self->getPixelHeight());

    return 1;
}

int Wrap_Video::getPixelDimensions(lua_State* L)
{
    auto* self = luax_checkvideo(L, 1);

    lua_pushnumber(L, self->getPixelWidth());
    lua_pushnumber(L, self->getPixelHeight());

    return 2;
}

int Wrap_Video::setFilter(lua_State* L)
{
    auto* self   = luax_checkvideo(L, 1);
    auto sampler = self->getSamplerState();

    const char* minStr = luaL_checkstring(L, 2);
    const char* magStr = luaL_optstring(L, 3, minStr);

    if (!SamplerState::getConstant(minStr, sampler.minFilter))
        return luax_enumerror(L, "filter mode", SamplerState::FilterModes, minStr);

    if (!SamplerState::getConstant(magStr, sampler.magFilter))
        return luax_enumerror(L, "filter mode", SamplerState::FilterModes, magStr);

    sampler.maxAnisotropy = std::min(std::max(1, (int)luaL_optnumber(L, 4, 1.0)), LOVE_UINT8_MAX);
    luax_catchexcept(L, [&]() { self->setSamplerState(sampler); });

    return 0;
}

int Wrap_Video::getFilter(lua_State* L)
{
    auto* self          = luax_checkvideo(L, 1);
    const auto& sampler = self->getSamplerState();

    std::string_view minStr {};
    std::string_view magStr {};

    if (!SamplerState::getConstant(sampler.minFilter, minStr))
        return luaL_error(L, "Unknown filter mode.");

    if (!SamplerState::getConstant(sampler.magFilter, magStr))
        return luaL_error(L, "Unknown filter mode.");

    luax_pushstring(L, minStr);
    luax_pushstring(L, magStr);
    lua_pushnumber(L, sampler.maxAnisotropy);

    return 3;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getStream",          Wrap_Video::getStream          },
    { "getSource",          Wrap_Video::getSource          },
    { "_setSource",         Wrap_Video::setSource          },
    { "getWidth",           Wrap_Video::getWidth           },
    { "getHeight",          Wrap_Video::getHeight          },
    { "getDimensions",      Wrap_Video::getDimensions      },
    { "getPixelWidth",      Wrap_Video::getPixelWidth      },
    { "getPixelHeight",     Wrap_Video::getPixelHeight     },
    { "getPixelDimensions", Wrap_Video::getPixelDimensions },
    { "setFilter",          Wrap_Video::setFilter          },
    { "getFilter",          Wrap_Video::getFilter          }
};
// clang-format on

namespace love
{
    Video* luax_checkvideo(lua_State* L, int index)
    {
        return luax_checktype<Video>(L, index);
    }

    int open_video(lua_State* L)
    {
        int result = luax_register_type(L, &Video::type, functions);

        luaL_loadbuffer(L, wrap_video_lua, sizeof(wrap_video_lua), "=[love \"wrap_Video.lua\"]");
        luax_gettypemetatable(L, Video::type);
        lua_call(L, 1, 0);

        return result;
    }
} // namespace love
