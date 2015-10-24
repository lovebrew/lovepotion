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

// http://hellomico.com/getting-started/convert-audio-to-raw/

#include "../shared.h"

#define CLASS_TYPE  LUAOBJ_TYPE_SOURCE
#define CLASS_NAME  "Source"

bool channelList[32];

int getOpenChannel() {

	int i;
	for (i = 0; i < 32; i++) {
		if (!channelList[i]) {
			channelList[i] = true;
			return i + 8;
		}
	}

	return NULL;

}

const char *sourceInit(love_source *self, const char *filename) {

	if (fileExists(filename)) {

		const char *ext = fileExtension(filename);

		if (strncmp(ext, "raw", 3) == 0) {

			FILE *file = fopen(filename, "rb");

			if (file) {

				fseek(file, 0, SEEK_END);
				self->size = ftell(file);
				fseek(file, 0, SEEK_SET);
				self->buffer = linearAlloc(self->size);

				if (!self->buffer) {
					fclose(file);
					return "Could not allocate sound buffer";
				}

				fread(self->buffer, 1, self->size, file);

				self->format = SOUND_FORMAT_16BIT;
				self->used = false;
				self->channel = getOpenChannel();
				self->samplerate = 44100;
				self->extension = ext;

			}

			fclose(file);

		} else if (strncmp(ext, "wav", 3) == 0) {

			FILE *file = fopen(filename, "rb");

			if (file) {

				u32 samplerate;

				fseek(file, 0, SEEK_END);
				self->size = ftell(file);
				fseek(file, 0, SEEK_SET);

				self->buffer = linearAlloc(self->size - 44);

				if (!self->buffer) {
					fclose(file);
					return "Could not allocate sound buffer";
				}

				fseek(file, 44, SEEK_SET);
				fread(self->buffer, 1, self->size - 44, file);
				fseek(file, 0, SEEK_SET);

				fseek(file, 24, SEEK_SET);
				fread(&self->samplerate, 1, 4, file);
				fseek(file, 0, SEEK_SET);

				self->format = SOUND_FORMAT_16BIT;
				self->used = false;
				self->channel = getOpenChannel();
				self->extension = ext;

			}

			fclose(file);

		} else return "Unknown audio type";

	} else return "Could not open source, does not exist.";

	return NULL;

}

int sourceNew(lua_State *L) { // love.audio.newSource()

	const char *filename = luaL_checkstring(L, 1);
	char final[strlen(rootDir) + strlen(filename) + 2];
	combine(final, rootDir, filename);

	love_source *self = luaobj_newudata(L, sizeof(*self));
	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *error = sourceInit(self, final);

	if (error) luaError(L, error);

	return 1;

}

int sourcePlay(lua_State *L) { // source:play()

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (!self || !self->buffer || !self->format || !soundEnabled) {

		luaError(L, "There was an error playing the source, sound may not be available.");
		return;

	}

	self->used = true;
	csndPlaySound(self->channel, self->format | SOUND_ONE_SHOT, self->samplerate, 1, 0, self->buffer, self->buffer, self->size);

	return 0;

}

int sourceStop(lua_State *L) { // source:stop()

	// TODO

	return 0;

}

int sourceIsPlaying(lua_State *L) { // source:isPlaying()

	// TODO

	return 0;

}

int sourceGC(lua_State *L) { // Garbage Collection

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (!self->buffer) return 0;

	linearFree(self->buffer);
	self->buffer = NULL;

	channelList[self->channel - 8] = false;

	return 0;

}

int initSourceClass(lua_State *L) {

	luaL_Reg reg[] = {
		{"new",			sourceNew	},
		{"__gc",		sourceGC	},
		{"play",		sourcePlay	},
		{"stop",		sourceStop	},
		{"isPlaying",	sourceIsPlaying},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, sourceNew, reg);

	return 1;
}
