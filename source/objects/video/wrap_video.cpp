#include "objects/video/wrap_video.h"
#include "wrap_video_lua.h"

using namespace love;

int Wrap_Video::GetStream(lua_State* L)
{
    Video* self = Wrap_Video::CheckVideo(L, 1);

    Luax::PushType(L, self->GetStream());

    return 1;
}

int Wrap_Video::GetSource(lua_State* L)
{
    Video* self    = Wrap_Video::CheckVideo(L, 1);
    Source* source = self->GetSource();

    if (source)
        Luax::PushType(L, source);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Video::SetSource(lua_State* L)
{
    Video* self = Wrap_Video::CheckVideo(L, 1);

    if (lua_isnoneornil(L, 2))
        self->SetSource(nullptr);
    else
    {
        Source* source = Luax::CheckType<Source>(L, 2);
        self->SetSource(source);
    }

    return 0;
}

int Wrap_Video::GetWidth(lua_State* L)
{
    Video* self = Wrap_Video::CheckVideo(L, 1);

    lua_pushnumber(L, self->GetWidth());

    return 1;
}

int Wrap_Video::GetHeight(lua_State* L)
{
    Video* self = Wrap_Video::CheckVideo(L, 1);

    lua_pushnumber(L, self->GetHeight());

    return 1;
}

int Wrap_Video::GetDimensions(lua_State* L)
{
    Video* self = Wrap_Video::CheckVideo(L, 1);

    lua_pushnumber(L, self->GetWidth());
    lua_pushnumber(L, self->GetHeight());

    return 2;
}

int Wrap_Video::GetPixelWidth(lua_State* L)
{
    Video* self = Wrap_Video::CheckVideo(L, 1);

    lua_pushnumber(L, self->GetPixelWidth());

    return 1;
}

int Wrap_Video::GetPixelHeight(lua_State* L)
{
    Video* self = Wrap_Video::CheckVideo(L, 1);

    lua_pushnumber(L, self->GetPixelHeight());

    return 1;
}

int Wrap_Video::GetPixelDimensions(lua_State* L)
{
    Video* self = Wrap_Video::CheckVideo(L, 1);

    lua_pushnumber(L, self->GetPixelWidth());
    lua_pushnumber(L, self->GetPixelHeight());

    return 2;
}

int Wrap_Video::SetFilter(lua_State* L)
{
    Video* self            = Wrap_Video::CheckVideo(L, 1);
    Texture::Filter filter = self->GetFilter();

    const char* min = luaL_checkstring(L, 2);
    const char* mag = luaL_checkstring(L, 3);

    if (!Texture::GetConstant(min, filter.min))
        return Luax::EnumError(L, "filter mode", Texture::GetConstants(filter.min), min);

    if (!Texture::GetConstant(mag, filter.mag))
        return Luax::EnumError(L, "filter mode", Texture::GetConstants(filter.mag), mag);

    filter.anisotropy = luaL_optnumber(L, 4, 1.0f);

    Luax::CatchException(L, [&]() { self->SetFilter(filter); });

    return 0;
}

int Wrap_Video::GetFilter(lua_State* L)
{
    Video* self                  = Wrap_Video::CheckVideo(L, 1);
    const Texture::Filter filter = self->GetFilter();

    const char* min = nullptr;
    const char* mag = nullptr;

    if (!Texture::GetConstant(filter.min, min))
        return luaL_error(L, "Unknown filter mode.");

    if (!Texture::GetConstant(filter.mag, mag))
        return luaL_error(L, "Unknown filter mode.");

    lua_pushstring(L, min);
    lua_pushstring(L, mag);

    lua_pushnumber(L, filter.anisotropy);

    return 3;
}

Video* Wrap_Video::CheckVideo(lua_State* L, int index)
{
    return Luax::CheckType<Video>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "_setSource",         Wrap_Video::SetSource          },
    { "getDimensions",      Wrap_Video::GetDimensions      },
    { "getFilter",          Wrap_Video::GetFilter          },
    { "getHeight",          Wrap_Video::GetHeight          },
    { "getPixelDimensions", Wrap_Video::GetPixelDimensions },
    { "getPixelHeight",     Wrap_Video::GetPixelHeight     },
    { "getPixelWidth",      Wrap_Video::GetPixelWidth      },
    { "getSource",          Wrap_Video::GetSource          },
    { "getStream",          Wrap_Video::GetStream          },
    { "getWidth",           Wrap_Video::GetWidth           },
    { "setFilter",          Wrap_Video::SetFilter          },
    { 0,                    0                              }
};
// clang-format on

int Wrap_Video::Register(lua_State* L)
{
    int result = Luax::RegisterType(L, &Video::type, functions, nullptr);

    Luax::RunWrapper(L, (const char*)wrap_video_lua, wrap_video_lua_size, "wrap_video.lua",
                     Video::type);

    return result;
}
