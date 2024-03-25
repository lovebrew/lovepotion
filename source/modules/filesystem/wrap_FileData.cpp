#include "modules/filesystem/wrap_FileData.hpp"

#include "modules/data/wrap_Data.hpp"

using namespace love;

int Wrap_FileData::clone(lua_State* L)
{
    auto* self      = luax_checkfiledata(L, 1);
    FileData* clone = nullptr;

    luax_catchexcept(L, [&]() { clone = self->clone(); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_FileData::getFilename(lua_State* L)
{
    auto* self = luax_checkfiledata(L, 1);

    luax_pushstring(L, self->getFilename());

    return 1;
}

int Wrap_FileData::getExtension(lua_State* L)
{
    auto* self = luax_checkfiledata(L, 1);

    luax_pushstring(L, self->getExtension());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",        Wrap_FileData::clone        },
    { "getFilename",  Wrap_FileData::getFilename  },
    { "getExtension", Wrap_FileData::getExtension }
};
// clang-format on

namespace love
{
    FileData* luax_checkfiledata(lua_State* L, int idx)
    {
        return luax_checktype<FileData>(L, idx);
    }

    int open_filedata(lua_State* L)
    {
        return luax_register_type(L, &FileData::type, Wrap_Data::functions, functions);
    }
} // namespace love
