#include "objects/texture/wrap_texture.h"
#include "common/luax.h"

using namespace love;

int Wrap_Texture::GetTextureType(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    const char* typeStr;
    const Texture::TextureType t = self->GetTextureType();

    if (!love::Texture::GetConstant(t, typeStr))
        return Luax::EnumError(L, "texture type",
                               love::Texture::GetConstants(love::Texture::TEXTURE_MAX_ENUM),
                               typeStr);

    lua_pushstring(L, typeStr);

    return 1;
}

int Wrap_Texture::GetWidth(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetWidth());

    return 1;
}

int Wrap_Texture::GetHeight(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetHeight());

    return 1;
}

int Wrap_Texture::GetDimensions(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetWidth());
    lua_pushnumber(L, self->GetHeight());

    return 2;
}

int Wrap_Texture::SetFilter(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    love::Texture::Filter filter = self->GetFilter();

    const char* min = luaL_checkstring(L, 2);
    const char* mag = luaL_optstring(L, 3, min);

    if (!love::Texture::GetConstant(min, filter.min))
        return Luax::EnumError(L, "filter mode", love::Texture::GetConstants(filter.min), min);

    if (!love::Texture::GetConstant(mag, filter.mag))
        return Luax::EnumError(L, "filter mode", love::Texture::GetConstants(filter.mag), mag);

    Luax::CatchException(L, [&]() { self->SetFilter(filter); });

    return 0;
}

int Wrap_Texture::GetFilter(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    const love::Texture::Filter filter = self->GetFilter();

    const char* min = nullptr;
    const char* mag = nullptr;

    if (!love::Texture::GetConstant(filter.min, min))
        return luaL_error(L, "Unknown filter mode.");

    if (!love::Texture::GetConstant(filter.mag, mag))
        return luaL_error(L, "Unknown filter mode.");

    lua_pushstring(L, min);
    lua_pushstring(L, mag);

    return 2;
}

int Wrap_Texture::SetWrap(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    love::Texture::Wrap wrap;

    const char* sstr = luaL_checkstring(L, 2);
    const char* tstr = luaL_optstring(L, 3, sstr);
    const char* rstr = luaL_optstring(L, 4, sstr);

    if (!love::Texture::GetConstant(sstr, wrap.s))
        return Luax::EnumError(L, "wrap mode", love::Texture::GetConstants(wrap.s), sstr);

    if (!love::Texture::GetConstant(tstr, wrap.t))
        return Luax::EnumError(L, "wrap mode", love::Texture::GetConstants(wrap.t), tstr);

    if (!love::Texture::GetConstant(rstr, wrap.r))
        return Luax::EnumError(L, "wrap mode", love::Texture::GetConstants(wrap.r), rstr);

    lua_pushboolean(L, self->SetWrap(wrap));

    return 1;
}

int Wrap_Texture::GetWrap(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    const love::Texture::Wrap wrap = self->GetWrap();

    const char* sstr = nullptr;
    const char* tstr = nullptr;
    const char* rstr = nullptr;

    if (!love::Texture::GetConstant(wrap.s, sstr))
        return luaL_error(L, "Unknown wrap mode.");

    if (!love::Texture::GetConstant(wrap.t, tstr))
        return luaL_error(L, "Unknown wrap mode.");

    if (!love::Texture::GetConstant(wrap.r, rstr))
        return luaL_error(L, "Unknown wrap mode.");

    lua_pushstring(L, sstr);
    lua_pushstring(L, tstr);
    lua_pushstring(L, rstr);

    return 3;
}

love::Texture* Wrap_Texture::CheckTexture(lua_State* L, int index)
{
    return Luax::CheckType<love::Texture>(L, index);
}

// clang-format off
const luaL_Reg Wrap_Texture::functions[9] =
{
    { "getTextureType", GetTextureType },
    { "getWidth",       GetWidth       },
    { "getHeight",      GetHeight      },
    { "getDimensions",  GetDimensions  },
    { "setFilter",      SetFilter      },
    { "getFilter",      GetFilter      },
    { "setWrap",        SetWrap        },
    { "getWrap",        GetWrap        },
    { 0,                0              }
};
// clang-format on

int Wrap_Texture::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Texture::type, functions, nullptr);
}
