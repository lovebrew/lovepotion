#pragma once

#include "common/luax.hpp"
#include "modules/filesystem/FileData.hpp"

namespace love
{
    FileData* luax_checkfiledata(lua_State* L, int index);

    int open_filedata(lua_State* L);
} // namespace love

namespace Wrap_FileData
{
    int clone(lua_State* L);

    int getFilename(lua_State* L);

    int getExtension(lua_State* L);
} // namespace Wrap_FileData
