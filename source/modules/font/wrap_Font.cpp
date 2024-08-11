#include "modules/font/wrap_Font.hpp"

#if defined(__3DS__)
    #include "modules/font/Font.hpp"
#else
    #include "modules/font/freetype/Font.hpp"
#endif

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

static Rasterizer::Settings luax_checktruetypesettings(lua_State* L, int index)
{
    Rasterizer::Settings settings {};

    if (lua_type(L, index))
    {
        const char* hinting = lua_isnoneornil(L, index) ? nullptr : luaL_checkstring(L, index);
        if (hinting && !Rasterizer::getConstant(hinting, settings.hinting))
            luax_enumerror(L, "Font hinting mode", Rasterizer::Hintings, hinting);

        if (!lua_isnoneornil(L, index + 1))
            settings.dpiScale.set((float)luaL_checknumber(L, index + 1));
    }
    else
    {
        luaL_checktype(L, index, LUA_TTABLE);

        lua_getfield(L, index, "hinting");
        if (!lua_isnoneornil(L, -1))
        {
            const char* hinting = luaL_checkstring(L, -1);
            if (!Rasterizer::getConstant(hinting, settings.hinting))
                luax_enumerror(L, "Font hinting mode", Rasterizer::Hintings, hinting);
        }
        lua_pop(L, 1);

        lua_getfield(L, index, "dpiscale");
        if (!lua_isnoneornil(L, -1))
            settings.dpiScale.set((float)luaL_checknumber(L, -1));
        lua_pop(L, 1);
    }

    return settings;
}

/*
** This is a simplified version(?) from 3.0 to make it suck less.
** We check the system font type *first*, but if it fails, we just get the file data via filename.
** Getting the data by filename will automatically propagate the error if the file doesn't exist.
**
** On Nintendo 3DS, we return a Data object wrapped through SystemFont because of linear memory.
*/
static Data* luax_checkfilename(lua_State* L, int index)
{
    // CFG_REGION_USA, PlSharedFontType_Standard, OS_SHAREDDATATYPE_FONT_STANDARD
    auto systemFontType = SystemFontType(0);
    const char* name    = luaL_checkstring(L, index);

    if (!FontModule::getConstant(name, systemFontType))
        return luax_getfiledata(L, index);

    return FontModule::loadSystemFontByType(systemFontType);
}

int Wrap_FontModule::newTrueTypeRasterizer(lua_State* L)
{
    Rasterizer* rasterizer = nullptr;

    if (lua_type(L, 1) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        int size = luaL_optinteger(L, 1, 13);
        Rasterizer::Settings settings {};

        if (!lua_isnoneornil(L, 2))
            settings = luax_checktruetypesettings(L, 2);

        luax_catchexcept(L, [&] { rasterizer = instance()->newTrueTypeRasterizer(size, settings); });
    }
    else
    {
        int size = luaL_optinteger(L, 2, 12);

        Rasterizer::Settings settings {};
        if (!lua_isnoneornil(L, 3))
            settings = luax_checktruetypesettings(L, 3);

        Data* data = nullptr;

        if (luax_istype(L, 1, Data::type))
        {
            data = luax_checkdata(L, 1);
            data->retain();
        }
        else
            data = luax_checkfilename(L, 1);

        // clang-format off
        luax_catchexcept(
            L, [&] { rasterizer = instance()->newTrueTypeRasterizer(data, size, settings); },
            [&](bool) { data->release(); }
        );
        // clang-format on
    }

    luax_pushtype(L, rasterizer);
    rasterizer->release();

    return 1;
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
