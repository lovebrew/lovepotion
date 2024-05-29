#include "modules/font/wrap_GlyphData.hpp"

using namespace love;

int Wrap_GlyphData::clone(lua_State* L)
{
    auto* self       = luax_checkglyphdata(L, 1);
    GlyphData* clone = nullptr;

    luax_catchexcept(L, [&] { clone = self->clone(); });
    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_GlyphData::getWidth(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    lua_pushinteger(L, self->getWidth());

    return 1;
}

int Wrap_GlyphData::getHeight(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    lua_pushinteger(L, self->getHeight());

    return 1;
}

int Wrap_GlyphData::getDimensions(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    lua_pushinteger(L, self->getWidth());
    lua_pushinteger(L, self->getHeight());

    return 2;
}

int Wrap_GlyphData::getGlyph(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    lua_pushinteger(L, (lua_Number)self->getGlyph());

    return 1;
}

int Wrap_GlyphData::getGlyphString(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    luax_catchexcept(L, [&] { luax_pushstring(L, self->getGlyphString()); });

    return 1;
}

int Wrap_GlyphData::getAdvance(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    lua_pushinteger(L, self->getAdvance());

    return 1;
}

int Wrap_GlyphData::getBearing(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    lua_pushinteger(L, self->getBearingX());
    lua_pushinteger(L, self->getBearingY());

    return 2;
}

int Wrap_GlyphData::getBoundingBox(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    int minX = self->getMinX();
    int minY = self->getMinY();
    int maxX = self->getMaxX();
    int maxY = self->getMaxY();

    int width  = maxX - minX;
    int height = maxY - minY;

    lua_pushinteger(L, minX);
    lua_pushinteger(L, minY);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);

    return 4;
}

int Wrap_GlyphData::getFormat(lua_State* L)
{
    auto* self = luax_checkglyphdata(L, 1);

    std::string_view format {};
    if (!getConstant(self->getFormat(), format))
        return luax_enumerror(L, "pixel format", format.data());

    luax_pushstring(L, format);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",           Wrap_GlyphData::clone          },
    { "getWidth",        Wrap_GlyphData::getWidth       },
    { "getHeight",       Wrap_GlyphData::getHeight      },
    { "getDimensions",   Wrap_GlyphData::getDimensions  },
    { "getGlyph",        Wrap_GlyphData::getGlyph       },
    { "getGlyphString",  Wrap_GlyphData::getGlyphString },
    { "getAdvance",      Wrap_GlyphData::getAdvance     },
    { "getBearing",      Wrap_GlyphData::getBearing     },
    { "getBoundingBox",  Wrap_GlyphData::getBoundingBox },
    { "getFormat",       Wrap_GlyphData::getFormat      }
};
// clang-format on

namespace love
{
    GlyphData* luax_checkglyphdata(lua_State* L, int index)
    {
        return luax_checktype<GlyphData>(L, index);
    }

    int open_glyphdata(lua_State* L)
    {
        return luax_register_type(L, &GlyphData::type, functions);
    }
} // namespace love
