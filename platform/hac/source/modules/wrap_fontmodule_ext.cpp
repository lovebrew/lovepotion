#include <modules/fontmodule_ext.hpp>

#include <objects/data/wrap_data.hpp>

#include <modules/font/wrap_fontmodule.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>

#include <utilities/functions.hpp>

using namespace love;

#define instance() (Module::GetInstance<FontModule<Console::Which>>(Module::M_FONT))

int Wrap_FontModule::NewTrueTypeRasterizer(lua_State* L)
{
    Rasterizer* self = nullptr;
    std::optional<TrueTypeRasterizer<>::Hinting> hinting;

    if (lua_type(L, 1) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        int size = luaL_optinteger(L, 1, 13);

        const char* hint = luaL_optstring(L, 2, "normal");

        if (!(hinting = TrueTypeRasterizer<>::hintings.Find(hint)))
        {
            return luax::EnumError(L, "TrueType font hinting mode", TrueTypeRasterizer<>::hintings,
                                   hint);
        }

        // clang-format off
        if (lua_isnoneornil(L, 3))
            luax::CatchException(L, [&]() { self = instance()->NewTrueTypeRasterizer(size, *hinting); });
        else
        {
            float dpiScale = luaL_checknumber(L, 3);
            luax::CatchException(L, [&]() { self = instance()->NewTrueTypeRasterizer(size, dpiScale, *hinting); });
        }
        // clang-format on
    }
    else
    {
        Data* data = nullptr;
        std::optional<PlSharedFontType> systemFont;

        if (luax::IsType(L, 1, Data::type))
        {
            data = Wrap_Data::CheckData(L, 1);
            data->Retain();
        }
        else
        {
            const char* filename = luaL_checkstring(L, 1);

            if (!love::has_file_extension(filename))
            {
                const auto systemFonts = FontModule<Console::HAC>::systemFonts;
                if (!(systemFont = systemFonts.Find(filename)))
                    return luax::EnumError(L, "system font name", systemFonts, filename);
            }
            else
                data = Wrap_Filesystem::GetFileData(L, 1);
        }

        int size = luaL_optinteger(L, 2, 12);

        const char* hint = luaL_optstring(L, 3, "normal");
        std::optional<TrueTypeRasterizer<>::Hinting> hinting;

        if (!(hinting = TrueTypeRasterizer<>::hintings.Find(hint)))
        {
            return luax::EnumError(L, "TrueType font hinting mode", TrueTypeRasterizer<>::hintings,
                                   hint);
        }

        if (lua_isnoneornil(L, 4))
        {
            // clang-format off
            if (systemFont)
            {
                luax::CatchException(L,
                    [&]() { self = instance()->NewTrueTypeRasterizer(size, *hinting, *systemFont);});
            }
            else
            {
                luax::CatchException(L,
                    [&]() { self = instance()->NewTrueTypeRasterizer(data, size, *hinting); },
                    [&](bool) { data->Release(); }
                );
            }
            // clang-format on
        }
        else
        {
            float dpiScale = luaL_checknumber(L, 4);

            // clang-format off
            luax::CatchException(L,
                [&]() { self = instance()->NewTrueTypeRasterizer(data, size, dpiScale, *hinting); },
                [&](bool) { data->Release(); }
            );
            // clang-format on
        }
    }

    luax::PushType(L, self);
    self->Release();

    return 1;
}

int Wrap_FontModule::NewSystemFontRasterizer(lua_State* L, uint8_t systemFont)
{
    ::Rasterizer* self = nullptr;
    const auto type    = (PlSharedFontType)systemFont;
    const auto hinting = TrueTypeRasterizer<>::HINTING_NORMAL;

    luax::CatchException(L, [&]() {
        self = instance()->NewTrueTypeRasterizer(13, hinting, (PlSharedFontType)type);
    });

    luax::PushType(L, self);
    self->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newTrueTypeRasterizer", Wrap_FontModule::NewTrueTypeRasterizer }
};
// clang-format on

std::span<const luaL_Reg> Wrap_FontModule::extensions = functions;
