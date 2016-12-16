// This code is licensed under the MIT Open Source License.

// Copyright (c) 2015 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <shared.h>

const char *fileInit(love_file *self, const char *filename);

int filesystemRead(lua_State *L) {

	const char * filename = luaL_checkstring(L, 1);

	FILE * file = fopen(filename, "rb");
		
	fseek(file, 0, SEEK_END);

	long fsize = ftell(file);

	fseek(file, 0, SEEK_SET);

	char *string = malloc(fsize + 1);

	fread(string, fsize, 1, file);

	lua_pushstring(L, string);

	fclose(file);

	return 1;
}

int filesystemCreateDirectory(lua_State *L) {

	const char * directoryName = luaL_checkstring(L, 1);

	mkdir(directoryName, 0777);
	
	return 0;
}

int filesystemGetDirectoryItems(lua_State *L) {

	const char * path = luaL_checkstring(L, 1);

	DIR * dp;

	dp = opendir (path);

	lua_newtable(L);

	int tablepos = 0;

	if (dp != NULL) {

		while (true) {
			struct dirent * ep;

			ep = readdir (dp);

			lua_pushstring(L, ep->d_name);

			tablepos++;

			lua_rawseti(L, -2, tablepos);

			if (!ep) {
				(void) closedir (dp);
				break;
			}
		}

		return 1;

	} else {

		return 0;

	}

	return 0;

}

int filesystemAppend(lua_State * L) {

	const char * filename = luaL_checkstring(L, 1);

	const char * data = luaL_checkstring(L, 2);

	size_t size = luaL_optnumber(L, 3, strlen(data));

	FILE * file = fopen(filename, "a");

	fwrite(data, 1, size, file);

	fflush(file);

	fclose(file);

	return 0;
}

int filesystemIsFile(lua_State *L) {

	const char * filename = luaL_checkstring(L, 1);

	struct stat info;

	if (stat(filename, &info) != 0) {

		if (S_ISREG(info.st_mode)) {

			lua_pushboolean(L, 1);

			return 1;
		}

		lua_pushboolean(L, 0);

		return 1;
	}

	return 1;
}

int filesystemRemove(lua_State *L) {

	const char * filename = luaL_checkstring(L, 1);

	remove(filename);

	return 0;

}

int filesystemWrite(lua_State *L) {

	const char * filename = luaL_checkstring(L, 1);

	const char * data = luaL_checkstring(L, 2);

	size_t size = luaL_optnumber(L, 3, strlen(data));

	FILE * file = fopen(filename, "w");

	fwrite(data, 1, size, file);

	return 0;

}

int filesystemIsDirectory(lua_State * L) {

	const char * filename = luaL_checkstring(L, 1);

	struct stat info;

	if (stat(filename, &info) != 0) {
		if (S_ISDIR(info.st_mode)) {
			lua_pushboolean(L, 1);

			return 1;
		}

		lua_pushboolean(L, 0);
		
		return 1;
	}

	return 1;
}

int filesystemLoad(lua_State *L) {

	const char * filename = luaL_checkstring(L, 1);

	luaL_loadfile(L, filename);

	return 1;

}

int filesystemSetIdentity(lua_State *L) {

	const char * newIdentity = luaL_checkstring(L, 1);

	identity = newIdentity;

	return 0;

}

int fileNew(lua_State *L);
int fileRead(lua_State *L);
int fileWrite(lua_State *L);
int fileClose(lua_State *L);
int fileFlush(lua_State *L);
int fileIsEOF(lua_State *L);
int fileOpen(lua_State *L);


int initLoveFilesystem(lua_State *L) {

	luaL_Reg reg[] = {
		{ "newFile", 		fileNew },
		{ "read", 		filesystemRead },
		{ "createDirectory", 		filesystemCreateDirectory },
		{ "getDirectoryItems",	filesystemGetDirectoryItems},
		{ "append",	filesystemAppend},
		{ "isFile",	filesystemIsFile},
		{ "isDirectory",	filesystemIsDirectory},
		{ "write",	filesystemWrite},
		{ "remove",	filesystemRemove},
		{ "setIdentity", filesystemSetIdentity},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}
