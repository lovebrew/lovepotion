#pragma once

#include "data/wrap_datamodule.h"
#include "objects/file/file.h"

namespace Wrap_File
{
    int Close(lua_State* L);

    int Flush(lua_State* L);

    int GetBuffer(lua_State* L);

    int GetFilename(lua_State* L);

    int GetMode(lua_State* L);

    int GetSize(lua_State* L);

    int IsEOF(lua_State* L);

    int IsOpen(lua_State* L);

    int Lines_I(lua_State* L);

    int Lines(lua_State* L);

    int Open(lua_State* L);

    int Read(lua_State* L);

    int Seek(lua_State* L);

    int SetBuffer(lua_State* L);

    int Tell(lua_State* L);

    int Write(lua_State* L);

    love::File* CheckFile(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_File
