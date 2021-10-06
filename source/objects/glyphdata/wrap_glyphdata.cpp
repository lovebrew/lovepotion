#include "objects/glyphdata/wrap_glyphdata.h"

using namespace love;

int Wrap_GlyphData::Clone(lua_State* L)
{
    love::GlyphData* self  = Wrap_GlyphData::CheckGlyphData(L, 1);
    love::GlyphData* clone = nullptr;

    Luax::CatchException(L, [&]() { clone = self->Clone(); });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_GlyphData::GetWidth(lua_State* L)
{
    love::GlyphData* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetWidth());

    return 1;
}

int Wrap_GlyphData::GetHeight(lua_State* L)
{
    love::GlyphData* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetHeight());

    return 1;
}

int Wrap_GlyphData::GetDimensions(lua_State* L)
{
    love::GlyphData* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetWidth());
    lua_pushinteger(L, self->GetHeight());

    return 2;
}

int Wrap_GlyphData::GetGlyph(lua_State* L)
{
    love::GlyphData* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushnumber(L, (lua_Number)self->GetGlyph());

    return 1;
}

int Wrap_GlyphData::GetGlyphString(lua_State* L)
{
    love::GlyphData* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    Luax::CatchException(L, [&]() { Luax::PushString(L, self->GetGlyphString()); });

    return 1;
}

int Wrap_GlyphData::GetAdvance(lua_State* L)
{
    love::GlyphData* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetAdvance());

    return 1;
}

int Wrap_GlyphData::GetBearing(lua_State* L)
{
    love::GlyphData* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetBearingX());
    lua_pushinteger(L, self->GetBearingY());

    return 2;
}

int Wrap_GlyphData::GetBoundingBox(lua_State* L)
{
    love::GlyphData* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    int minX = self->GetMinX();
    int minY = self->GetMinY();
    int maxX = self->GetMaxX();
    int maxY = self->GetMaxY();

    int width  = maxX - minX;
    int height = maxY - minY;

    lua_pushinteger(L, minX);
    lua_pushinteger(L, minY);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);

    return 4;
}

love::GlyphData* Wrap_GlyphData::CheckGlyphData(lua_State* L, int index)
{
    return Luax::CheckType<love::GlyphData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",          Wrap_GlyphData::Clone          },
    { "getAdvance",     Wrap_GlyphData::GetAdvance     },
    { "getBearing",     Wrap_GlyphData::GetBearing     },
    { "getBoundingBox", Wrap_GlyphData::GetBoundingBox },
    { "getDimensions",  Wrap_GlyphData::GetDimensions  },
    { "getGlyph",       Wrap_GlyphData::GetGlyph       },
    { "getGlyphString", Wrap_GlyphData::GetGlyphString },
    { "getHeight",      Wrap_GlyphData::GetHeight      },
    { "getWidth",       Wrap_GlyphData::GetWidth       },
    { 0,                0                              }
};
// clang-format on

int Wrap_GlyphData::Register(lua_State* L)
{
    return Luax::RegisterType(L, &GlyphData::type, Wrap_Data::functions, functions, nullptr);
}
