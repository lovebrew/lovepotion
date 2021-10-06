#include "modules/font/wrap_fontmodule.h"
#include "common/luax.h"

#include <filesystem>

using namespace love;

#define instance() (Module::GetInstance<FontModule>(Module::M_FONT))

int Wrap_FontModule::NewRasterizer(lua_State* L)
{
    /* First or second arg is a number */
    if (lua_type(L, 1) == LUA_TNUMBER || lua_type(L, 2) == LUA_TNUMBER || lua_isnone(L, 1))
#if defined(__3DS__)
        return Wrap_FontModule::NewBCFNTRasterizer(L);
#else
        return Wrap_FontModule::NewTrueTypeRasterizer(L);
#endif
    else if (lua_isnoneornil(L, 2))
    {
        Rasterizer* self = nullptr;

        FileData* fileData = Wrap_Filesystem::GetFileData(L, 1);

        Luax::CatchException(
            L, [&]() { self = instance()->NewRasterizer(fileData); },
            [&](bool) { fileData->Release(); });

        Luax::PushType(L, self);
        self->Release();
    }

    return 1;
}

#if defined(__3DS__)
int Wrap_FontModule::NewBCFNTRasterizer(lua_State* L)
{
    Rasterizer* self = nullptr;

    if (lua_type(L, 1) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        int size = luaL_optinteger(L, 1, 12);

        Luax::CatchException(L, [&]() { self = instance()->NewBCFNTRasterizer(size); });
    }
    else
    {
        Data* data                        = nullptr;
        common::Font::SystemFontType type = common::Font::SystemFontType::TYPE_MAX_ENUM;

        if (Luax::IsType(L, 1, love::Data::type))
        {
            data = Wrap_Data::CheckData(L, 1);
            data->Retain();
        }
        else
        {
            const char* str = luaL_checkstring(L, 1);

            if (std::filesystem::path(str).extension().empty())
            {
                if (!Font::GetConstant(str, type))
                    return Luax::EnumError(L, "font type", Font::GetConstants(type), str);
            }
            else /* load font from a file, *.ttf usually */
                data = Wrap_Filesystem::GetFileData(L, 1);
        }

        int size = (int)luaL_optinteger(L, 2, 12);

        if (type == Font::SystemFontType::TYPE_MAX_ENUM)
            Luax::CatchException(
                L, [&]() { self = instance()->NewBCFNTRasterizer(data, size); },
                [&](bool) { data->Release(); });
        else
            Luax::CatchException(L, [&]() { self = instance()->NewBCFNTRasterizer(size, type); });
    }

    Luax::PushType(L, self);
    self->Release();

    return 1;
}
#endif

#if defined(__SWITCH__)
int Wrap_FontModule::NewTrueTypeRasterizer(lua_State* L)
{
    Rasterizer* self = nullptr;

    TrueTypeRasterizer::Hinting hinting = TrueTypeRasterizer::HINTING_NORMAL;

    if (lua_type(L, 1) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        int size = (int)luaL_optinteger(L, 1, 12);

        const char* hintstr = lua_isnoneornil(L, 2) ? nullptr : luaL_checkstring(L, 2);
        if (hintstr && !TrueTypeRasterizer::GetConstant(hintstr, hinting))
            return Luax::EnumError(L, "TrueType font hinting mode",
                                   TrueTypeRasterizer::GetConstants(hinting), hintstr);

        if (lua_isnoneornil(L, 3))
            Luax::CatchException(
                L, [&]() { self = instance()->NewTrueTypeRasterizer(size, hinting); });
        else
        {
            float dpiscale = (float)luaL_checknumber(L, 3);
            Luax::CatchException(
                L, [&]() { self = instance()->NewTrueTypeRasterizer(size, dpiscale, hinting); });
        }
    }
    else
    {
        love::Data* data              = nullptr;
        Font::SystemFontType fontType = Font::SystemFontType::TYPE_MAX_ENUM;

        if (Luax::IsType(L, 1, love::Data::type))
        {
            data = Wrap_Data::CheckData(L, 1);
            data->Retain();
        }
        else
        {
            const char* str = luaL_checkstring(L, 1);

            if (std::filesystem::path(str).extension().empty())
            {
                if (!Font::GetConstant(str, fontType))
                    return Luax::EnumError(L, "font type", Font::GetConstants(fontType), str);
            }
            else /* load font from a file */
                data = Wrap_Filesystem::GetFileData(L, 1);
        }

        int size = (int)luaL_optinteger(L, 2, 12);

        const char* hintstr = lua_isnoneornil(L, 3) ? nullptr : luaL_checkstring(L, 3);
        if (hintstr && !TrueTypeRasterizer::GetConstant(hintstr, hinting))
            return Luax::EnumError(L, "TrueType font hinting mode",
                                   TrueTypeRasterizer::GetConstants(hinting), hintstr);

        if (lua_isnoneornil(L, 4))
        {
            if (fontType != Font::SystemFontType::TYPE_MAX_ENUM)
                Luax::CatchException(L, [&]() {
                    self = instance()->NewTrueTypeRasterizer(fontType, size, hinting);
                });
            else
                Luax::CatchException(
                    L, [&]() { self = instance()->NewTrueTypeRasterizer(data, size, hinting); },
                    [&](bool) { data->Release(); });
        }
        else
        {
            float dpiScale = (float)luaL_checknumber(L, 4);

            if (fontType != Font::SystemFontType::TYPE_MAX_ENUM)
                Luax::CatchException(L, [&]() {
                    self = instance()->NewTrueTypeRasterizer(fontType, size, dpiScale, hinting);
                });
            else
                Luax::CatchException(
                    L,
                    [&]() {
                        self = instance()->NewTrueTypeRasterizer(data, size, dpiScale, hinting);
                    },
                    [&](bool) { data->Release(); });
        }
    }

    Luax::PushType(L, self);
    self->Release();

    return 1;
}
#endif

int Wrap_FontModule::NewGlyphData(lua_State* L)
{
    Rasterizer* rasterizer = Wrap_Rasterizer::CheckRasterizer(L, 1);
    GlyphData* glyphData   = nullptr;

    // newGlyphData accepts a unicode character or a codepoint number.
    if (lua_type(L, 2) == LUA_TSTRING)
    {
        std::string glyph = Luax::CheckString(L, 2);
        Luax::CatchException(L, [&]() { glyphData = instance()->NewGlyphData(rasterizer, glyph); });
    }
    else
    {
        uint32_t glyph = (uint32_t)luaL_checknumber(L, 2);
        glyphData      = instance()->NewGlyphData(rasterizer, glyph);
    }

    Luax::PushType(L, glyphData);
    glyphData->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
#if defined(__3DS__)
    { "newBCFNTRasterizer",    Wrap_FontModule::NewBCFNTRasterizer    },
#endif
    { "newGlyphData",          Wrap_FontModule::NewGlyphData          },
#if defined(__SWICH__)
    { "newTrueTypeRasterizer", Wrap_FontModule::NewTrueTypeRasterizer },
#endif
    { "newRasterizer",         Wrap_FontModule::NewRasterizer         },
    {  0,                      0                                      }
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
    FontModule* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new FontModule(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "font";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = types;

    return Luax::RegisterModule(L, wrappedModule);
}
