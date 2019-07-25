/*
** modules/filesystem.h
** @brief : Filesystem operations (read/write files)
*/

#pragma once

enum AssetLocation
{
    ROMFS_DEV,
    DIRECTORY,
    FILE_ASSOC
};

class Filesystem
{
    public:
        Filesystem() = delete;

        static void Initialize(char * path);

        static std::string GetSaveDirectory();

        static int Register(lua_State * L);

        static void Exit();

    private:
        static AssetLocation GetAssetLocation(char * path);

        // Löve2D Functions

        static int Read(lua_State * L);
        static int Write(lua_State * L);

        static int GetInfo(lua_State * L);

        static int SetIdentity(lua_State * L);
        static int GetIdentity(lua_State * L);

        static int GetSaveDirectory(lua_State * L);

        static int CreateDirectory(lua_State * L);

        static int GetDirectoryItems(lua_State * L);

        static int Load(lua_State * L);

        static int Remove(lua_State * L);

        // End Löve2D Functions

        static std::string Redirect(const char * path);
};
