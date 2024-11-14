#include "modules/font/wrap_Rasterizer.hpp"

using namespace love;

int Wrap_Rasterizer::getHeight(lua_State* L)
{
    auto* self = luax_checkrasterizer(L, 1);

    lua_pushinteger(L, self->getHeight());

    return 1;
}

int Wrap_Rasterizer::getAdvance(lua_State* L)
{
    auto* self = luax_checkrasterizer(L, 1);

    lua_pushinteger(L, self->getAdvance());

    return 1;
}

int Wrap_Rasterizer::getAscent(lua_State* L)
{
    auto* self = luax_checkrasterizer(L, 1);

    lua_pushinteger(L, self->getAscent());

    return 1;
}

int Wrap_Rasterizer::getDescent(lua_State* L)
{
    auto* self = luax_checkrasterizer(L, 1);

    lua_pushinteger(L, self->getDescent());

    return 1;
}

int Wrap_Rasterizer::getLineHeight(lua_State* L)
{
    auto* self = luax_checkrasterizer(L, 1);

    lua_pushinteger(L, self->getLineHeight());

    return 1;
}

int Wrap_Rasterizer::getGlyphData(lua_State* L)
{
    auto* self      = luax_checkrasterizer(L, 1);
    GlyphData* data = nullptr;

    luax_catchexcept(L, [&] {
        if (lua_type(L, 2) == LUA_TSTRING)
        {
            auto glyph = luax_checkstring(L, 2);
            data       = self->getGlyphData(glyph);
        }
        else
        {
            auto codepoint = (uint32_t)luaL_checknumber(L, 2);
            data           = self->getGlyphData(codepoint);
        }
    });

    luax_pushtype(L, data);
    data->release();

    return 1;
}

int Wrap_Rasterizer::getGlyphCount(lua_State* L)
{
    auto* self = luax_checkrasterizer(L, 1);

    lua_pushinteger(L, self->getGlyphCount());

    return 1;
}

int Wrap_Rasterizer::hasGlyphs(lua_State* L)
{
    auto* self = luax_checkrasterizer(L, 1);

    bool hasGlyph = false;

    int count = lua_gettop(L) - 1;
    count     = count < 1 ? 1 : count;

    luax_catchexcept(L, [&]() {
        for (int index = 2; index < count + 2; index++)
        {
            if (lua_type(L, index) == LUA_TSTRING)
                hasGlyph = self->hasGlyphs(luax_checkstring(L, index));
            else
                hasGlyph = self->hasGlyph((uint32_t)luaL_checknumber(L, index));

            if (!hasGlyph)
                break;
        }
    });

    luax_pushboolean(L, hasGlyph);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getHeight",      Wrap_Rasterizer::getHeight     },
    { "getAdvance",     Wrap_Rasterizer::getAdvance    },
    { "getAscent",      Wrap_Rasterizer::getAscent     },
    { "getDescent",     Wrap_Rasterizer::getDescent    },
    { "getLineHeight",  Wrap_Rasterizer::getLineHeight },
    { "getGlyphData",   Wrap_Rasterizer::getGlyphData  },
    { "getGlyphCount",  Wrap_Rasterizer::getGlyphCount },
    { "hasGlyphs",      Wrap_Rasterizer::hasGlyphs     }
};
// clang-format on

namespace love
{
    Rasterizer* luax_checkrasterizer(lua_State* L, int index)
    {
        return luax_checktype<Rasterizer>(L, index);
    }

    int open_rasterizer(lua_State* L)
    {
        return luax_register_type(L, &Rasterizer::type, functions);
    }
} // namespace love
