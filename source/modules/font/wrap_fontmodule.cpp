#include <modules/font/wrap_fontmodule.hpp>

#include <objects/glyphdata/wrap_glyphdata.hpp>
#include <objects/rasterizer/wrap_rasterizer.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>
#include <objects/data/wrap_data.hpp>

#include <modules/fontmodule_ext.hpp>

using namespace love;

#define instance() (Module::GetInstance<FontModule<Console::Which>>(Module::M_FONT))

int Wrap_FontModule::NewRasterizer(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TNUMBER || lua_type(L, 2) == LUA_TNUMBER || lua_isnone(L, 1))
        return Wrap_FontModule::NewTrueTypeRasterizer(L);
    else if (lua_isnoneornil(L, 2))
    {
        Rasterizer* self = nullptr;

        const char* name           = luaL_checkstring(L, 1);
        const auto systemFontValid = FontModule<Console::Which>::systemFonts.Find(name);

        if (!systemFontValid)
        {
            auto* fileData = Wrap_Filesystem::GetFileData(L, 1);

            luax::CatchException(
                L, [&]() { self = instance()->NewRasterizer(fileData); },
                [&](bool) { fileData->Release(); });

            luax::PushType(L, self);
            self->Release();

            return 1;
        }
        else
            return 0;
    }

    return 0;
}

int Wrap_FontModule::NewTrueTypeRasterizer(lua_State* L)
{
    Rasterizer* rasterizer = nullptr;
    std::optional<TrueTypeRasterizer<>::Hinting> hinting(
        TrueTypeRasterizer<>::Hinting::HINTING_NORMAL);

    if (lua_type(L, 1) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        int size = luaL_optinteger(L, 1, 13);

        const char* hintingStr = lua_isnoneornil(L, 2) ? nullptr : luaL_checkstring(L, 2);

        if (hintingStr)
        {
            const auto hintings = TrueTypeRasterizer<>::hintings;
            if (!(hinting = TrueTypeRasterizer<>::hintings.Find(hintingStr)))
                return luax::EnumError(L, "TrueType font hinting mode", hintings, hintingStr);
        }

        if (lua_isnoneornil(L, 3))
        {
            luax::CatchException(
                L, [&]() { rasterizer = instance()->NewTrueTypeRasterizer(size, *hinting); });
        }
        else
        {
            float dpiScale = luaL_checknumber(L, 3);
            luax::CatchException(L, [&]() {
                rasterizer = instance()->NewTrueTypeRasterizer(size, dpiScale, *hinting);
            });
        }
    }
    else
    {
        Data* data = nullptr;

        if (luax::IsType(L, 1, Data::type))
        {
            data = Wrap_Data::CheckData(L, 1);
            data->Retain();
        }
        else
            data = Wrap_Filesystem::GetFileData(L, 1);

        int size = luaL_optinteger(L, 2, 12);

        const char* hintingStr = lua_isnoneornil(L, 3) ? nullptr : luaL_checkstring(L, 3);
        if (hintingStr)
        {
            const auto hintings = TrueTypeRasterizer<>::hintings;
            if (!(hinting = TrueTypeRasterizer<>::hintings.Find(hintingStr)))
                return luax::EnumError(L, "TrueType font hinting mode", hintings, hintingStr);
        }

        if (lua_isnoneornil(L, 4))
        {
            luax::CatchException(
                L, [&]() { rasterizer = instance()->NewTrueTypeRasterizer(data, size, *hinting); },
                [&](bool) { data->Release(); });
        }
        else
        {
            float dpiScale = luaL_checknumber(L, 4);
            luax::CatchException(
                L,
                [&]() {
                    rasterizer = instance()->NewTrueTypeRasterizer(data, size, dpiScale, *hinting);
                },
                [&](bool) { data->Release(); });
        }
    }

    return 0;
}

int Wrap_FontModule::NewGlyphData(lua_State* L)
{
    auto* rasterizer = Wrap_Rasterizer::CheckRasterizer(L, 1);
    GlyphData* self  = nullptr;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        std::string glyph = luax::CheckString(L, 2);
        luax::CatchException(L, [&]() { self = instance()->NewGlyphData(rasterizer, glyph); });
    }
    else
    {
        uint32_t glyph = (uint32_t)luaL_checknumber(L, 2);
        self           = instance()->NewGlyphData(rasterizer, glyph);
    }

    luax::PushType(L, self);
    self->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newGlyphData",          Wrap_FontModule::NewGlyphData          },
    { "newRasterizer",         Wrap_FontModule::NewRasterizer         },
    { "newTrueTypeRasterizer", Wrap_FontModule::NewTrueTypeRasterizer }
};

static constexpr lua_CFunction types[] =
{
    Wrap_GlyphData::Register,
    Wrap_Rasterizer::Register,
    nullptr
};
// clang-format on

int Wrap_FontModule::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new FontModule<Console::Which>(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "font";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = types;

    return luax::RegisterModule(L, wrappedModule);
}
