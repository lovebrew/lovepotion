#include "objects/filedata/wrap_filedata.h"

using namespace love;

int Wrap_FileData::Clone(lua_State* L)
{
    FileData* data  = Wrap_FileData::CheckFileData(L, 1);
    FileData* clone = nullptr;

    Luax::CatchException(L, [&]() { clone = data->Clone(); });
    Luax::PushType(L, clone);

    clone->Release();

    return 1;
}

int Wrap_FileData::GetFilename(lua_State* L)
{
    FileData* self = Wrap_FileData::CheckFileData(L, 1);

    lua_pushstring(L, self->GetFilename().c_str());

    return 1;
}

int Wrap_FileData::GetExtension(lua_State* L)
{
    FileData* self = Wrap_FileData::CheckFileData(L, 1);

    lua_pushstring(L, self->GetExtension().c_str());

    return 1;
}

FileData* Wrap_FileData::CheckFileData(lua_State* L, int index)
{
    return Luax::CheckType<FileData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",        Wrap_FileData::Clone        },
    { "getExtension", Wrap_FileData::GetExtension },
    { "getFilename",  Wrap_FileData::GetFilename  },
    { 0,              0                           }
};
// clang-format on

int Wrap_FileData::Register(lua_State* L)
{
    return Luax::RegisterType(L, &FileData::type, Wrap_Data::functions, functions, nullptr);
}
