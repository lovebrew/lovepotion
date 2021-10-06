#include "objects/rasterizer/wrap_rasterizer.h"

using namespace love;

int Wrap_Rasterizer::GetHeight(lua_State* L)
{
    Rasterizer* self = Wrap_Rasterizer::CheckRasterizer(L, 1);

    lua_pushinteger(L, self->GetHeight());

    return 1;
}

int Wrap_Rasterizer::GetAdvance(lua_State* L)
{
    Rasterizer* self = Wrap_Rasterizer::CheckRasterizer(L, 1);

    lua_pushinteger(L, self->GetAdvance());

    return 1;
}

int Wrap_Rasterizer::GetAscent(lua_State* L)
{
    Rasterizer* self = Wrap_Rasterizer::CheckRasterizer(L, 1);

    lua_pushinteger(L, self->GetAscent());

    return 1;
}

int Wrap_Rasterizer::GetDescent(lua_State* L)
{
    Rasterizer* self = Wrap_Rasterizer::CheckRasterizer(L, 1);

    lua_pushinteger(L, self->GetDescent());

    return 1;
}

int Wrap_Rasterizer::GetLineHeight(lua_State* L)
{
    Rasterizer* self = Wrap_Rasterizer::CheckRasterizer(L, 1);

    lua_pushinteger(L, self->GetLineHeight());

    return 1;
}

int Wrap_Rasterizer::GetGlyphData(lua_State* L)
{
    Rasterizer* self             = Wrap_Rasterizer::CheckRasterizer(L, 1);
    common::GlyphData* glyphData = nullptr;

    /*
    ** getGlyphData accepts a unicode character
    ** or a codepoint number
    */
    Luax::CatchException(L, [&]() {
        if (lua_type(L, 2) == LUA_TSTRING)
        {
            std::string glyph = Luax::CheckString(L, 2);
            glyphData         = self->GetGlyphData(glyph);
        }
        else
        {
            uint32_t glyph = (uint32_t)luaL_checknumber(L, 2);
            glyphData      = self->GetGlyphData(glyph);
        }
    });

    Luax::PushType(L, glyphData);
    glyphData->Release();

    return 1;
}

int Wrap_Rasterizer::GetGlyphCount(lua_State* L)
{
    Rasterizer* self = Wrap_Rasterizer::CheckRasterizer(L, 1);

    lua_pushinteger(L, self->GetGlyphCount());

    return 1;
}

int Wrap_Rasterizer::HasGlyphs(lua_State* L)
{
    Rasterizer* self = Wrap_Rasterizer::CheckRasterizer(L, 1);

    bool hasGlyph = false;

    int count = lua_gettop(L) - 1;
    count     = (count < 1) ? 1 : count;

    Luax::CatchException(L, [&]() {
        for (int i = 2; i < count + 2; i++)
        {
            if (lua_type(L, i) == LUA_TSTRING)
                hasGlyph = self->HasGlyphs(Luax::CheckString(L, i));
            else
                hasGlyph = self->HasGlyph((uint32_t)luaL_checknumber(L, i));

            if (!hasGlyph)
                break;
        }
    });

    lua_pushboolean(L, hasGlyph);

    return 1;
}

Rasterizer* Wrap_Rasterizer::CheckRasterizer(lua_State* L, int index)
{
    return Luax::CheckType<Rasterizer>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getAdvance",    Wrap_Rasterizer::GetAdvance    },
    { "getAscent",     Wrap_Rasterizer::GetAscent     },
    { "getDescent",    Wrap_Rasterizer::GetDescent    },
    { "getGlyphCount", Wrap_Rasterizer::GetGlyphCount },
    { "getGlyphData",  Wrap_Rasterizer::GetGlyphData  },
    { "getHeight",     Wrap_Rasterizer::GetHeight     },
    { "getLineHeight", Wrap_Rasterizer::GetLineHeight },
    { "hasGlyphs",     Wrap_Rasterizer::HasGlyphs     },
    { 0,               0                              }
};
// clang-format on

int Wrap_Rasterizer::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Rasterizer::type, functions, nullptr);
}
