#include <modules/font/wrap_fontmodule.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>
#include <modules/fontmodule_ext.hpp>

#include <objects/glyphdata/wrap_glyphdata.hpp>

#if defined(__WIIU__)
std::span<const luaL_Reg> Wrap_FontModule::extensions;
#elif defined(__SWITCH__)
int Wrap_FontModule::NewBCFNTRasterizer(lua_State* L)
{
    return 0;
}
#elif defined(__3DS__)
int Wrap_FontModule::NewTrueTypeRasterizer(lua_State* L)
{
    return 0;
}
#endif

using namespace love;

using Rasterizer = love::Rasterizer<Console::Which>;
#define instance() (Module::GetInstance<FontModule<Console::Which>>(Module::M_FONT))

int Wrap_FontModule::NewRasterizer(lua_State* L)
{
    if (lua_type(L, 1) == LUA_TNUMBER || lua_type(L, 2) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        if (Console::Is(Console::CTR))
            return Wrap_FontModule::NewBCFNTRasterizer(L);

        return Wrap_FontModule::NewTrueTypeRasterizer(L);
    }
    else if (lua_isnoneornil(L, 2))
    {
        ::Rasterizer* self = nullptr;

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
            return Wrap_FontModule::NewSystemFontRasterizer(L, *systemFontValid);
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
    { "newGlyphData",  Wrap_FontModule::NewGlyphData  },
    { "newRasterizer", Wrap_FontModule::NewRasterizer }
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

    wrappedModule.instance          = instance;
    wrappedModule.name              = "font";
    wrappedModule.functions         = functions;
    wrappedModule.extendedFunctions = extensions;
    wrappedModule.type              = &Module::type;
    wrappedModule.types             = types;

    return luax::RegisterModule(L, wrappedModule);
}
