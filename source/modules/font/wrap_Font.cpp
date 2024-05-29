#include "modules/font/wrap_Font.hpp"

#include "modules/font/font.hpp"

#include "modules/font/wrap_GlyphData.hpp"
#include "modules/font/wrap_Rasterizer.hpp"

#include "modules/filesystem/wrap_Filesystem.hpp"

using namespace love;

#define instance() (Module::getInstance<FontModule>(Module::M_FONT))

int Wrap_FontModule::newRasterizer(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TNUMBER || lua_type(L, 2) == LUA_TNUMBER || lua_isnone(L, 1))
        return newTrueTypeRasterizer(L);
    else if (lua_isnoneornil(L, 2))
    {
        Rasterizer* rasterizer = nullptr;
        auto* fileData         = luax_getfiledata(L, 1);

        // clang-format off
        luax_catchexcept(L,
            [&]() { rasterizer = instance()->newRasterizer(fileData); },
            [&](bool) { fileData->release(); }
        );
        // clang-format on

        luax_pushtype(L, rasterizer);
        rasterizer->release();

        return 1;
    }
    else
        return newBMFontRasterizer(L);
}

int Wrap_FontModule::newTrueTypeRasterizer(lua_State* L)
{
    return 0;
}

int Wrap_FontModule::newBMFontRasterizer(lua_State* L)
{
    return 0;
}

int Wrap_FontModule::newGlyphData(lua_State* L)
{
    auto* rasterizer = luax_checkrasterizer(L, 1);
    GlyphData* data  = nullptr;

    if (lua_type(L, 1) == LUA_TSTRING)
    {
        auto glyph = luax_checkstring(L, 2);
        luax_catchexcept(L, [&] { data = rasterizer->getGlyphData(glyph); });
    }
    else
    {
        auto glyph = (uint32_t)luaL_checknumber(L, 2);
        data       = instance()->newGlyphData(rasterizer, glyph);
    }

    luax_pushtype(L, data);
    data->release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newRasterizer",         Wrap_FontModule::newRasterizer         },
    { "newGlyphData",          Wrap_FontModule::newGlyphData          },
    { "newTrueTypeRasterizer", Wrap_FontModule::newTrueTypeRasterizer },
    { "newBMFontRasterizer",   Wrap_FontModule::newBMFontRasterizer   }
};

static constexpr lua_CFunction types[] =
{
    love::open_glyphdata,
    love::open_rasterizer
};
// clang-format on

int Wrap_FontModule::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new FontModule(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "font";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
