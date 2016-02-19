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

#define CLASS_TYPE  LUAOBJ_TYPE_FILE
#define CLASS_NAME  "File"

const char * fileInit(love_file *self, const char *filename) {

	if (!luaL_checkstring(L, 1)) {
		return "Bad argument #1 to newFile: string expected";
	}

	self->file = fopen(filename, "w");
	self->filename = filename;

	if (self->file) {
		fclose(self->file);
	}

	return NULL;

}

int fileNew(lua_State *L) {

	love_file *self = luaobj_newudata(L, sizeof(*self));
	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *filename = luaL_checkstring(L, 1);

	const char *error = fileInit(self, filename);

	if (error) {
		luaError(L, error);
	}

	return 1;

}

int fileWrite(lua_State *L) {

	love_file *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	char * data = luaL_checkstring(L, 2);

	fwrite(data, 1, strlen(data), self->file);

	return 0;

}

int fileRead(lua_State *L) {

	love_file *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (self && self->file) {

		fseek(self->file, 0, SEEK_END);

		long fsize = ftell(self->file);

		fseek(self->file, 0, SEEK_SET);

		char *string = malloc(fsize + 1);

		fread(string, fsize, 1, self->file);

		lua_pushstring(L, string);

		lua_pushinteger(L, (int)sizeof(string));

		return 2;

	}

	return  0;

}

int fileClose(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	fclose(self->file);

	return 0;

}

int fileOpen(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * mode = luaL_checkstring(L, 2);

	self->file = fopen(self->filename, mode);

	return 0;

}

int fileIsEOF(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	int isEOF = 0;

	if ( (ftell(self->file) > 0) ) {
		isEOF = 1;
	}

	lua_pushboolean(L, isEOF);

	return 1;

}

int fileFlush(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	fflush(self->file);

	return 0;

}

int fileGetBuffer(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushstring(L, self->mode);

	return 1;

}

int fileSetBuffer(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * mode = luaL_checkstring(L, 2);

	int buffmode;

	int size = luaL_optnumber(L, 3, 0);
	
	if (strcmp(mode, "none") == 0) {
		buffmode = _IONBF;
	} else if (strcmp(mode, "line") == 0) {
		buffmode = _IOLBF;
	} else if (strcmp(mode, "full") == 0) {
		buffmode = _IOFBF;
	}

	setvbuf(self->file, NULL, buffmode, size);

	self->mode = mode;

	return 0;

}

int fileGetFilename(lua_State *L) {
	
	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushstring(L, self->filename);

	return 1;

}

int fileGetSize(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	fseek(self->file, 0, SEEK_END);

	self->size = ftell(self->file);

	fseek(self->file, 0, SEEK_SET);

	lua_pushinteger(L, (int)self->size);

	return 1;

}

int fileIsOpen(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (!self->file) {
		
		lua_pushboolean(L, 0);

		return 1;

	}

	lua_pushboolean(L, 1);

	return 1;

}

int fileSeek(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	int position = luaL_checkinteger(L, 2);

	fseek(self->file, (long)position, SEEK_SET);

	self->position = (long)position;

	return 1;

}

int fileTell(lua_State *L) {

	love_file * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushinteger(L, (int)self->position);

	return 1;

}


/*const char * readLine (FILE * file) {

	const char * line = NULL, c;

	int count = 0;

	while (true) {

		line = (char *) realloc (line, (count + 1) * sizeof (char));

		if ((c = getc (file)) == '\n' || feof (file)) {

			line[count++] = '\0';

			break;

		} else

			line[count++] = c;

		}	

	}

	return line;

}

int fileLines (lua_State *L) {

	const char * line;

	love_file * self = luaobj_checkudata (L, 1, CLASS_TYPE);

	if (!self->file || feof (self->file)) {

		lua_pushnil (L);

		return 1;

	}

	line = readLine (self->file);

	lua_pushstring (L, line);

	free (line);
	
	return 1;

}*/

int initFileClass(lua_State *L) {

	luaL_Reg reg[] = {
		{ "new", 	fileNew},
		{ "write", fileWrite	},
		{ "read", fileRead	},
		{ "open", fileOpen	},
		{ "close", fileClose	},
		{ "isEOF", fileIsEOF	},
		{ "flush", fileFlush	},
		{ "setBuffer", fileGetBuffer	},
		{ "getBuffer", fileSetBuffer	},
		{ "getFilename", fileGetFilename	},
		{ "getSize", fileGetSize	},
		{ "isOpen", fileIsOpen	},
		{ "seek", fileSeek	},
		{ "tell", fileTell	},
		//{ "lines", fileLines	},
		{ 0, 0 },
	};

  luaobj_newclass(L, CLASS_NAME, NULL, fileNew, reg);

  return 1;

}