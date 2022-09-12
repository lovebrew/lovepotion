#pragma once

#include <common/luax.hpp>

#include <objects/data/filedata/filedata.hpp>
#include <objects/data/wrap_data.hpp>

namespace Wrap_FileData
{
    int Clone(lua_State* L);

    int GetFilename(lua_State* L);

    int GetExtension(lua_State* L);

    love::FileData* CheckFileData(lua_State* L, int index);

    love::FileData* GetFileData(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_FileData
