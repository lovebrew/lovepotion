#pragma once

#include "objects/data/wrap_data.h"
#include "objects/filedata/filedata.h"

namespace Wrap_FileData
{
    int Clone(lua_State* L);

    int GetFilename(lua_State* L);

    int GetExtension(lua_State* L);

    love::FileData* CheckFileData(lua_State* L, int index);

    love::FileData* GetFileData(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_FileData
