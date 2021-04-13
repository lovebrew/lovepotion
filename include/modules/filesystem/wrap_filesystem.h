#pragma once

#include "common/data.h"
#include "modules/data/wrap_datamodule.h"
#include "modules/filesystem/filesystem.h"
#include "objects/file/wrap_file.h"
#include "objects/filedata/wrap_filedata.h"

bool SetupWriteDirectory();
namespace Wrap_Filesystem
{
    love::Data* GetData(lua_State* L, int index);

    int Init(lua_State* L);

    int Append(lua_State* L);

    int CreateDirectory(lua_State* L);

    int GetDirectoryItems(lua_State* L);

    int GetIdentity(lua_State* L);

    int GetRealDirectory(lua_State* L);

    int SetRequirePath(lua_State* L);

    int GetUserDirectory(lua_State* L);

    int Lines(lua_State* L);

    int Load(lua_State* L);

    int GetInfo(lua_State* L);

    int GetSaveDirectory(lua_State* L);

    int GetExecutablePath(lua_State* L);

    int GetRequirePath(lua_State* L);

    int Loader(lua_State* L);

    int Load(lua_State* L);

    int NewFile(lua_State* L);

    int NewFileData(lua_State* L);

    int SetFused(lua_State* L);

    int IsFused(lua_State* L);

    int SetSource(lua_State* L);

    int GetSource(lua_State* L);

    int GetSourceBaseDirectory(lua_State* L);

    int GetWorkingDirectory(lua_State* L);

    int Mount(lua_State* L);

    int UnMount(lua_State* L);

    int Read(lua_State* L);

    int Register(lua_State* L);

    int Remove(lua_State* L);

    int Remove(lua_State* L);

    int SetIdentity(lua_State* L);

    int WriteOrAppend(lua_State* L, love::File::Mode mode);

    love::File* GetFile(lua_State* L, int index);

    love::FileData* GetFileData(lua_State* L, int index);

    bool CanGetData(lua_State* L, int index);

    int Write(lua_State* L);

    std::string Redirect(const char* path);
} // namespace Wrap_Filesystem
