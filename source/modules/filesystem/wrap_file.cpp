#include "common/runtime.h"
#include "file.h"

#define CLASS_TYPE  LUAOBJ_TYPE_FILE
#define CLASS_NAME  "File"

using love::File;

int fileNew(lua_State * L)
{
	const char * path = luaL_checkstring(L, 1);

	File * self = (File *)luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char * error = self->Init(path);
	
	if (error)
		console->ThrowError(error);
 
	return 1;
}

int fileOpen(lua_State * L)
{
	File * self = (File *)luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * mode = luaL_checkstring(L, 2);

	if (mode == nullptr)
		luaL_error(L, "Invalid mode");
	else
		self->Open(mode);

	return 0;
}

int fileWrite(lua_State * L)
{
	File * self = (File *)luaobj_checkudata(L, 1, CLASS_TYPE);

	size_t len;
	char * data = strdup(luaL_checklstring(L, 2, &len));

	self->Write(data, len);

	return 0;
}

int fileFlush(lua_State * L)
{
	File * self = (File *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Flush();

	return 0;
}

int fileRead(lua_State * L)
{
	File * self = (File *)luaobj_checkudata(L, 1, CLASS_TYPE);

	char * buffer = strdup(self->Read());

	lua_pushstring(L, buffer);

	return 1;
}

int fileGetSize(lua_State * L)
{
	File * self = (File *)luaobj_checkudata(L, 1, CLASS_TYPE);

	int size = self->GetSize();

	lua_pushnumber(L, size);

	return 1;
}

int fileIsOpen(lua_State * L)
{
	File * self = (File *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushboolean(L, self->IsOpen());

	return 1;
}

int fileGetMode(lua_State * L)
{
	File * self = (File *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushstring(L, self->GetMode());

	return 1;
}

int fileClose(lua_State * L)
{
	File * self = (File *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Close();

	return 0;
}

int fileGC(lua_State * L)
{
	return 0;
}

int initFileClass(lua_State *L) {

	luaL_Reg reg[] = {
		{"new",			fileNew				},
		{"open",		fileOpen	},
		{"read",		fileRead	},
		{"write",		fileWrite	},
		{"flush",		fileFlush	},
		{"close",		fileClose	},
		{"getSize",		fileGetSize	},
		{"isOpen",		fileIsOpen	},
		{"getMode",		fileGetMode	},
		{"__gc",		fileGC				},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, fileNew, reg);

	return 1;

}