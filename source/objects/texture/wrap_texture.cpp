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

    SamplerState state = self->GetSamplerState();

    const char* min = luaL_checkstring(L, 2);
    const char* mag = luaL_optstring(L, 3, min);

    if (!SamplerState::GetConstant(min, state.minFilter))
        return Luax::EnumError(L, "filter mode", SamplerState::GetConstants(state.minFilter), min);

    if (!SamplerState::GetConstant(mag, state.magFilter))
        return Luax::EnumError(L, "filter mode", SamplerState::GetConstants(state.magFilter), mag);

    state.maxAnisotropy = std::min(std::max(1, (int)luaL_optnumber(L, 4, 1.0)),
                                   (int)std::numeric_limits<uint8_t>::max());

    Luax::CatchException(L, [&]() { self->SetSamplerState(state); });

    return 0;
}

int Wrap_Texture::GetFilter(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    const SamplerState& state = self->GetSamplerState();

    const char* min = nullptr;
    const char* mag = nullptr;

    if (!SamplerState::GetConstant(state.minFilter, min))
        return luaL_error(L, "Unknown filter mode.");

    if (!SamplerState::GetConstant(state.magFilter, mag))
        return luaL_error(L, "Unknown filter mode.");

    lua_pushstring(L, min);
    lua_pushstring(L, mag);
    lua_pushnumber(L, state.maxAnisotropy);

    return 3;
}

int Wrap_Texture::SetWrap(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    SamplerState state = self->GetSamplerState();

    const char* ustr = luaL_checkstring(L, 2);
    const char* vstr = luaL_optstring(L, 3, ustr);
    const char* wstr = luaL_optstring(L, 4, ustr);

    if (!SamplerState::GetConstant(ustr, state.wrapU))
        return Luax::EnumError(L, "wrap mode", SamplerState::GetConstants(state.wrapU), ustr);

    if (!SamplerState::GetConstant(vstr, state.wrapV))
        return Luax::EnumError(L, "wrap mode", SamplerState::GetConstants(state.wrapV), vstr);

    if (!SamplerState::GetConstant(wstr, state.wrapW))
        return Luax::EnumError(L, "wrap mode", SamplerState::GetConstants(state.wrapW), wstr);

    Luax::CatchException(L, [&]() { self->SetSamplerState(state); });

    return 0;
}

int Wrap_Texture::GetWrap(lua_State* L)
{
    love::Texture* self = Wrap_Texture::CheckTexture(L, 1);

    const SamplerState& state = self->GetSamplerState();

    const char* ustr = nullptr;
    const char* vstr = nullptr;
    const char* wstr = nullptr;

    if (!SamplerState::GetConstant(state.wrapU, ustr))
        return Luax::EnumError(L, "wrap mode", SamplerState::GetConstants(state.wrapU), ustr);

    if (!SamplerState::GetConstant(state.wrapV, vstr))
        return Luax::EnumError(L, "wrap mode", SamplerState::GetConstants(state.wrapV), vstr);

    if (!SamplerState::GetConstant(state.wrapW, wstr))
        return Luax::EnumError(L, "wrap mode", SamplerState::GetConstants(state.wrapW), wstr);

    lua_pushstring(L, ustr);
    lua_pushstring(L, vstr);
    lua_pushstring(L, wstr);

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
