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

//http://hellomico.com/getting-started/convert-audio-to-raw/

#include "../shared.h"

#define CLASS_TYPE  LUAOBJ_TYPE_SOURCE
#define CLASS_NAME  "Source"

int channel;

const char *sourceInit(love_source *self, const char *filename) {
	
	if (!fileExists(filename)) {
		return "Could not open source. Does not exist.";
	}

	const char *ext = fileExtension(filename);

	if (strncmp(ext, "raw", 3) == 0) {

		FILE* file = fopen(filename, "rb+");

		if (!file) return "Failure to open source.";

		u8* buffer;
		long lsize;
		fseek(file, 0, SEEK_END);
		lsize = ftell(file);
		rewind(file);
		buffer = (u8*)malloc(lsize);

		self->size = lsize;

		if(!buffer) {
			fclose(file);
			return "Could not allocate sound buffer";
		}
			
		fread(buffer, 1, lsize, file);
		fclose(file);

		self->data = buffer;
		self->used = false;
		self->format = SOUND_FORMAT_16BIT;

	}

	return NULL;

}

void sourcePlay(lua_State *L) { //source:play()

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	printf("\n Attempting to play sound ...");
	printf(soundEnabled ? "true" : "false");

	if (!self || !self->data || !self->format || !soundEnabled) return;
	
	channel++;
	channel%=8;

	self->used = true;
	csndPlaySound(channel+8, self->format, 22050, 1.0, 0.0, (u32*)self->data, (u32*)self->data, self->size);

	printf("\n Played sound.");

}


int sourceNew(lua_State *L) { //love.audio.newSource()

	const char *filename = luaL_checkstring(L, 1);
	char final[strlen(rootDir) + strlen(filename) + 2];
	combine(final, rootDir, filename);

	love_source *self = luaobj_newudata(L, sizeof(*self));
	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *error = sourceInit(self, final);

	if (error) luaError(L, error);

	return 1;

}

int initSourceClass(lua_State *L) {

	luaL_Reg reg[] = {
		{"new", sourceNew},
		{"play", sourcePlay}
	};

	luaobj_newclass(L, CLASS_NAME, NULL, sourceNew, reg);

	return 1;
}
