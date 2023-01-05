#include <modules/font/wrap_fontmodule.hpp>
#include <modules/fontmodule_ext.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>
#include <objects/data/wrap_data.hpp>

#include <utilities/functions.hpp>

#include <3ds.h>

using namespace love;

using Rasterizer = love::Rasterizer<Console::CTR>;
#define instance() (Module::GetInstance<FontModule<Console::Which>>(Module::M_FONT))

int Wrap_FontModule::NewBCFNTRasterizer(lua_State* L)
{
    ::Rasterizer* self = nullptr;

    if (lua_type(L, 1) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        int size = luaL_optinteger(L, 1, 12);

        luax::CatchException(L, [&]() { self = instance()->NewBCFNTRasterizer(size); });
    }
    else
    {
        Data* data = nullptr;
        std::optional<CFG_Region> systemFont;

        if (luax::IsType(L, 1, love::Data::type))
        {
            data = Wrap_Data::CheckData(L, 1);
            data->Retain();
        }
        else
        {
            const char* filename = luaL_checkstring(L, 1);

            if (love::has_file_extension(filename))
            {
                const auto systemFonts = FontModule<Console::CTR>::systemFonts;
                if (!(systemFont = systemFonts.Find(filename)))
                    return luax::EnumError(L, "system font name", systemFonts, filename);
            }
            else
                data = Wrap_Filesystem::GetFileData(L, 1);
        }

        int size = (int)luaL_optinteger(L, 2, 12);

        if (!systemFont)
        {
            luax::CatchException(
                L, [&]() { self = instance()->NewBCFNTRasterizer(data, size); },
                [&](bool) { data->Release(); });
        }
        else
            luax::CatchException(
                L, [&]() { self = instance()->NewBCFNTRasterizer(size, (CFG_Region)*systemFont); });
    }

    luax::PushType(L, self);
    self->Release();

    return 1;
}

int Wrap_FontModule::NewSystemFontRasterizer(lua_State* L, uint8_t systemFont)
{
    ::Rasterizer* self = nullptr;
    const auto type    = (CFG_Region)systemFont;

    luax::CatchException(L, [&]() { self = instance()->NewBCFNTRasterizer(13, type); });

    luax::PushType(L, self);
    self->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newBCFNTRasterizer", Wrap_FontModule::NewBCFNTRasterizer }
};
// clang-format on

std::span<const luaL_Reg> Wrap_FontModule::extensions = functions;
