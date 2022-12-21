#include <modules/font/wrap_fontmodule.hpp>
#include <modules/fontmodule_ext.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>
#include <objects/data/wrap_data.hpp>

#include <utilities/functions.hpp>

#include <switch.h>

using Rasterizer = love::Rasterizer<love::Console::HAC>;
using namespace love;

#define instance() (Module::GetInstance<FontModule<Console::Which>>(Module::M_FONT))

int Wrap_FontModule::NewTrueTypeRasterizer(lua_State* L)
{
    ::Rasterizer* self = nullptr;

    std::optional<::Rasterizer::Hinting> hinting = ::Rasterizer::HINTING_NORMAL;

    if (lua_type(L, 1) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        int size         = luaL_optinteger(L, 1, 13);
        const char* hint = lua_isnoneornil(L, 2) ? nullptr : luaL_checkstring(L, 2);

        if (hint && (!(hinting = ::Rasterizer::hintings.Find(hint))))
            throw luax::EnumError(L, "TrueType font hinting mode", ::Rasterizer::hintings, hint);

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

        if (luax::IsType(L, 1, Data::type))
        {
            data = Wrap_Data::CheckData(L, 1);
            data->Retain();
        }
        else
            data = Wrap_Filesystem::GetFileData(L, 1);

        int size         = luaL_optinteger(L, 2, 12);
        const char* hint = luaL_checkstring(L, 3);

        if (hint && (!(hinting = ::Rasterizer::hintings.Find(hint))))
            throw luax::EnumError(L, "TrueType font hinting mode", ::Rasterizer::hintings, hint);

        // clang-format off
        if (lua_isnoneornil(L, 4))
        {
            luax::CatchException(L,
                [&]() { self = instance()->NewTrueTypeRasterizer(data, size, *hinting); },
                [&](bool) { data->Release(); }
            );
        }
        else
        {
            float dpiScale = luaL_checknumber(L, 4);
            luax::CatchException(L,
                [&]() { self = instance()->NewTrueTypeRasterizer(data, size, dpiScale, *hinting); },
                [&](bool) { data->Release(); }
            );
        }
        // clang-format on
    }

    luax::PushType(L, self);
    self->Release();

    return 1;
}
