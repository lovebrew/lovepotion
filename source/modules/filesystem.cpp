#include "common/runtime.h"
#include "modules/filesystem.h"

#include "objects/file/wrap_file.h"

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

bool ROMFS_INIT = true;
string SAVE_DIR = "sdmc:/switch/LovePotion";
string IDENTITY = "SuperGame";

FILE * logFile;

bool Filesystem::Initialize()
{
	//ROMFS_INIT = romfsInit() ? false : true;

	logFile = fopen("log.txt", "w+");

	if (!ROMFS_INIT)
		return false;
	else
	{
		char cwd[256];
		
		getcwd(cwd, 256);
		strcat(cwd, "game/");
		
		//chdir("romfs:/");
		chdir(cwd);
	}

	mkdir(SAVE_DIR.c_str(), 0777);
	
	return true;
}

//Löve2D Functions

int Filesystem::Read(lua_State * L)
{
	string path = Redirect(luaL_checkstring(L, 1));
	size_t size = 0;
	char * buffer;

	FILE * fileHandle = fopen(path.c_str(), "r");

	if (!fileHandle)
	{
		fclose(fileHandle);
		return 0;
	}

	fseek(fileHandle, 0, SEEK_END);
	size = ftell(fileHandle);
	rewind(fileHandle);

	buffer = (char *)malloc(size * sizeof(char));

	fread(buffer, 1, size, fileHandle);

	fclose(fileHandle);

	buffer[size] = '\0';

	lua_pushstring(L, buffer);

	free(buffer);

	return 1;
}

int Filesystem::Write(lua_State * L)
{
	string path = GetSaveDirectory() + string(luaL_checkstring(L, 1));
	string data = string(luaL_checkstring(L, 2));

	FILE * fileHandle = fopen(path.c_str(), "w");

	if (!fileHandle)
	{
		fclose(fileHandle);
		return 0;
	}

	fwrite((char *)data.data(), 1, data.length(), fileHandle);

	fflush(fileHandle);

	fclose(fileHandle);

	return 0;
}

int Filesystem::IsFile(lua_State * L)
{
	string path = Redirect(luaL_checkstring(L, 1));

	struct stat pathInfo;
	stat(path.c_str(), &pathInfo);

	lua_pushboolean(L, S_ISREG(pathInfo.st_mode));

	return 1;
}

int Filesystem::IsDirectory(lua_State * L)
{
	string path = Redirect(luaL_checkstring(L, 1));

	struct stat pathInfo;
	stat(path.c_str(), &pathInfo);

	lua_pushboolean(L, S_ISDIR(pathInfo.st_mode));

	return 1;
}

int Filesystem::SetIdentity(lua_State * L)
{
	IDENTITY = string(luaL_checkstring(L, 1));

	return 0;
}

int Filesystem::GetIdentity(lua_State * L)
{
	lua_pushstring(L, IDENTITY.c_str());

	return 1;
}

int Filesystem::CreateDirectory(lua_State * L)
{
	string path = GetSaveDirectory() + string(luaL_checkstring(L, 1));

	mkdir(path.c_str(), 0777);

	return 0;
}

int Filesystem::GetSaveDirectory(lua_State * L)
{
	lua_pushstring(L, GetSaveDirectory().c_str());

	return 1;
}

int Filesystem::GetSize(lua_State * L)
{
	string path = Redirect(luaL_checkstring(L, 1));

	struct stat pathInfo;
	int success = stat(path.c_str(), &pathInfo);

	if (success != 0)
	{
		lua_pushnil(L);
		return 1;
	}

	lua_pushnumber(L, pathInfo.st_size);

	return 1;
}

int Filesystem::GetDirectoryItems(lua_State * L)
{
	string path = Redirect(luaL_checkstring(L, 1));

	DIR * dp;

	dp = opendir(path.c_str());

	int tablepos = 0;

	if (dp != NULL) 
	{
		lua_newtable(L);

		struct dirent * ep;
		
		while ((ep = readdir(dp)) != NULL) 
		{
			if (ep->d_name[0] == '.')
				continue; //skip .

			tablepos++;

			lua_pushnumber(L, tablepos);
			lua_pushstring(L, ep->d_name);

			lua_settable(L, -3);
		}

		closedir(dp);

		return 1;
	}

	return 0;
}

int Filesystem::Remove(lua_State * L)
{
	string path = GetSaveDirectory() + string(luaL_checkstring(L, 1));

	remove(path.c_str());

	return 0;
}

//End Löve2D Functions

string Filesystem::GetSaveDirectory()
{
	return SAVE_DIR + "/" + IDENTITY + "/";
}

string Filesystem::Redirect(const char * path)
{
	struct stat pathInfo;
	int success = stat(path, &pathInfo);

	if (success == 0)
		return string(path);
	else
		return GetSaveDirectory() + string(path);
}

void Filesystem::LogToFile(const string & data)
{
	fwrite((char *)data.data(), 1, data.length(), logFile);

	fflush(logFile);
}

void Filesystem::CloseLog()
{
	fclose(logFile);
}

//Register Functions
int Filesystem::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "newFile",				fileNew			 },
		{ "read",					Read			 },
		{ "write",					Write			 },
		{ "isFile",					IsFile			 },
		{ "isDirectory",			IsDirectory		 },
		{ "createDirectory",		CreateDirectory	 },
		{ "getSaveDirectory",		GetSaveDirectory },
		{ "setIdentity",			SetIdentity		 },
		{ "getIdentity",			GetIdentity		 },
		{ "getDirectoryItems",		GetDirectoryItems},
		{ "remove",					Remove			 },
		{ "getSize",				GetSize			 },
		{ 0, 0 }
	};

	luaL_newlib(L, reg);
	
	return 1;
}