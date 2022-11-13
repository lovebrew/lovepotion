#include <objects/glyphdata/wrap_glyphdata.hpp>

#include <objects/data/wrap_data.hpp>

using namespace love;

GlyphData* Wrap_GlyphData::CheckGlyphData(lua_State* L, int index)
{
    return luax::CheckType<love::GlyphData>(L, index);
}

int Wrap_GlyphData::Clone(lua_State* L)
{
    auto* self       = Wrap_GlyphData::CheckGlyphData(L, 1);
    GlyphData* clone = nullptr;

    luax::CatchException(L, [&]() { clone = self->Clone(); });

    luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_GlyphData::GetWidth(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetWidth());

    return 1;
}

int Wrap_GlyphData::GetHeight(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetHeight());

    return 1;
}

int Wrap_GlyphData::GetDimensions(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetWidth());
    lua_pushinteger(L, self->GetHeight());

    return 2;
}

int Wrap_GlyphData::GetGlyph(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushnumber(L, (lua_Number)self->GetGlyph());

    return 1;
}

int Wrap_GlyphData::GetGlyphString(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    luax::CatchException(L, [&]() { luax::PushString(L, self->GetGlyphString()); });

    return 1;
}

int Wrap_GlyphData::GetAdvance(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetAdvance());

    return 1;
}

int Wrap_GlyphData::GetBearing(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    lua_pushinteger(L, self->GetBearingX());
    lua_pushinteger(L, self->GetBearingY());

    return 2;
}

int Wrap_GlyphData::GetBoundingBox(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

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

int Wrap_GlyphData::GetFormat(lua_State* L)
{
    auto* self = Wrap_GlyphData::CheckGlyphData(L, 1);

    std::optional<const char*> format;
    if (!(format = pixelFormats.ReverseFind(self->GetFormat())))
        return luax::EnumError(L, "pixel format", *format);

    lua_pushstring(L, *format);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",          Wrap_GlyphData::Clone          },
    { "getAdvance",     Wrap_GlyphData::GetAdvance     },
    { "getBearing",     Wrap_GlyphData::GetBearing     },
    { "getBoundingBox", Wrap_GlyphData::GetBoundingBox },
    { "getDimensions",  Wrap_GlyphData::GetDimensions  },
    { "getFormat",      Wrap_GlyphData::GetFormat      },
    { "getGlyph",       Wrap_GlyphData::GetGlyph       },
    { "getGlyphString", Wrap_GlyphData::GetGlyphString },
    { "getHeight",      Wrap_GlyphData::GetHeight      },
    { "getWidth",       Wrap_GlyphData::GetWidth       }
};
// clang-format on

int Wrap_GlyphData::Register(lua_State* L)
{
    return luax::RegisterType(L, &GlyphData::type, Wrap_Data::functions, functions);
}
