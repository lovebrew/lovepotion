#pragma once

#include "common/luax.hpp"
#include "modules/filesystem/physfs/File.hpp"

namespace love
{
    File* luax_checkfile(lua_State* L, int index);

    int open_file(lua_State* L);
} // namespace love

namespace Wrap_File
{
    int getSize(lua_State* L);

    int open(lua_State* L);

    int close(lua_State* L);

    int isOpen(lua_State* L);

    int read(lua_State* L);

    int write(lua_State* L);

    int flush(lua_State* L);

    int isEOF(lua_State* L);

    int tell(lua_State* L);

    int seek(lua_State* L);

    int lines_i(lua_State* L);

    int lines(lua_State* L);

    int setBuffer(lua_State* L);

    int getBuffer(lua_State* L);

    int getMode(lua_State* L);

    int getFilename(lua_State* L);

    int getExtension(lua_State* L);

    extern const luaL_Reg w_File_functions[16];
} // namespace Wrap_File
