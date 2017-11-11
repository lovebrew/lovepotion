#include "common/runtime.h"
#include "filesystem.h"
#include "wrap_file.h"
#include "file.h"

love::Filesystem * love::Filesystem::instance = 0;

const char * love::Filesystem::identity = "SuperGame";

using love::Filesystem;

Filesystem::Filesystem() {}

int Filesystem::SetIdentity(lua_State * L)
{
	const char * gameID = luaL_checkstring(L, 1);

	if (gameID != nullptr)
		identity = gameID;
	
	return 0;
}

int Filesystem::GetSaveDirectory(lua_State * L)
{
	lua_pushstring(L, Filesystem::Instance()->GetSaveDirectory());

	return 1;
}

int Filesystem::GetIdentity(lua_State * L)
{
	lua_pushstring(L, Filesystem::Instance()->GetIdentity());
	
	return 1;
}

int Filesystem::GetSize(lua_State * L)
{
	const char * filePath = luaL_checkstring(L, 1);
	struct stat fileInfo;

	if (filePath == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}
	
	if(stat(filePath, &fileInfo) != 0) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushnumber(L, fileInfo.st_size);

	return 1;
}

int Filesystem::IsFile(lua_State * L)
{
	const char * path = Filesystem::Instance()->Redirect(luaL_checkstring(L, 1));
	struct stat pathInfo;

	stat(path, &pathInfo);

	lua_pushboolean(L, S_ISREG(pathInfo.st_mode));

	return 1;
}

int Filesystem::Write(lua_State * L)
{
	const char * path = Filesystem::Instance()->Redirect(luaL_checkstring(L, 1));

	size_t len;
	const char * data = luaL_checklstring(L, 2, &len);
	
	love::File * file = new love::File;
	file->InitPath(path);

	if (!file->Open("w"))
		return 0;

	file->Write(data, len);
	file->Flush();
	file->Close();

	delete file;

	return 0;
}

int Filesystem::Read(lua_State * L)
{
	const char * path = Filesystem::Instance()->Redirect(luaL_checkstring(L, 1));
	char * buffer = nullptr;

	love::File * file = new love::File;
	file->InitPath(path);

	if (!file->Open("r"))
		return 0;

	buffer = file->Read();

	lua_pushstring(L, buffer);

	delete file;

	return 1;
}

int Filesystem::IsDirectory(lua_State * L)
{
	const char * path = Filesystem::Instance()->Redirect(luaL_checkstring(L, 1));
	struct stat pathInfo;

	stat(path, &pathInfo);

	lua_pushboolean(L, S_ISDIR(pathInfo.st_mode));
	
	return 1;
}

int Filesystem::CreateDirectory(lua_State * L)
{
	const char * path = Filesystem::Instance()->Redirect(luaL_checkstring(L, 1));

	mkdir(path, 0777);
	
	return 0;
}

int Filesystem::GetDirectoryItems(lua_State * L)
{
	const char * path = Filesystem::Instance()->Redirect(luaL_checkstring(L, 1));

	DIR * dp;

	dp = opendir(path);

	int tablepos = 0;

	if (dp != NULL) 
	{
		lua_newtable(L);

		struct dirent * ep;
		
		while (ep = readdir(dp)) 
		{
			if (ep->d_name[0] == '.')
				continue; //skip .

			std::string checkPath = path;
			checkPath += "/";
			checkPath.append(ep->d_name);
			
			if (Filesystem::Instance()->Exists(checkPath.c_str()))
			{
				tablepos++;

				lua_pushnumber(L, tablepos);
				lua_pushstring(L, ep->d_name);

				lua_settable(L, -3);
			}
		}

		closedir(dp);

		return 1;

	}

	return 0;
}

int Filesystem::Load(lua_State * L)
{
	const char * path = luaL_checkstring(L, 1);

	luaL_loadfile(L, path);

	return 1;
}

bool Filesystem::Exists(const char * path)
{
	path = this->Redirect(path);

	struct stat pathInfo;
	stat(path, &pathInfo);

	return S_ISDIR(pathInfo.st_mode) || S_ISREG(pathInfo.st_mode);
}

const char * Filesystem::Redirect(const char * path)
{
	char * filepath = nullptr;

	struct stat pathInfo;
	int success = stat(path, &pathInfo);

	if (success == 0)
		return path;
	else
	{
		filepath = (char *)malloc(strlen(Filesystem::GetSaveDirectory()) + strlen(path) + 1);

		strcpy(filepath, Filesystem::GetSaveDirectory());
		strcat(filepath, path);

		return filepath;
	}
}

char * Filesystem::GetSaveDirectory()
{
	static char buf[0x80] = {0};

	if (!*buf)
		snprintf(buf, sizeof(buf), "sdmc:/3ds/data/LovePotion/%s/", identity);

	return buf;
}

const char * Filesystem::GetIdentity()
{
	return identity;
}

int filesystemInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "newFile",			fileNew								},
		{ "setIdentity",		love::Filesystem::SetIdentity		},
		{ "getIdentity",		love::Filesystem::GetIdentity		},
		{ "getSaveDirectory",	love::Filesystem::GetSaveDirectory	},
		{ "getSize",			love::Filesystem::GetSize			},
		{ "isFile",				love::Filesystem::IsFile			},
		{ "isDirectory",		love::Filesystem::IsDirectory		},
		{ "createDirectory",	love::Filesystem::CreateDirectory	},
		{ "getDirectoryItems",	love::Filesystem::GetDirectoryItems },
		{ "read",				love::Filesystem::Read				},
		{ "write",				love::Filesystem::Write				},
		{ "load",				love::Filesystem::Load				},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}