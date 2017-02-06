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

bool channelList[24];
love_source * streams[24];

int getOpenChannel() {

	for (int i = 0; i <= 23; i++) {
		if (!channelList[i]) {
			channelList[i] = true;
			return i;
		}
	}

	return -1;

}

void updateStreams() {
	
	for (int i = 0; i <= 23; i++) {
		if (streams[i] != NULL) {
			love_source * self = streams[i];

			if (!ndspChnIsPlaying(self->audiochannel)) {
				if (!self->eof) {
					//linearFree(self->data);

					if (self->type == TYPE_OGG) {
						if (ov_seekable(&self->vorbisFile) && ov_raw_tell(&self->vorbisFile) != self->position)
							ov_raw_seek(&self->vorbisFile, self->position);

						char * err = sourceOggDecodeStream(self, true);

						if (err != NULL)
							printf("\nError: %s\n", err);

						sourceStreamPlay(self);
					}
					
				}
			}
		} else {
			continue;
		}
	}

}

#define CLASS_TYPE  LUAOBJ_TYPE_SOURCE
#define CLASS_NAME  "Source"

const char *sourceInit(love_source *self, const char *filename, const char * type) {

	if (fileExists(filename)) {

		for (int i=0; i<12; i++) self->mix[i] = 1.0f;
		self->interp = NDSP_INTERP_LINEAR;

		const char *ext = fileExtension(filename);

		if (strncmp(ext, "wav", 3) == 0) {

			self->type = TYPE_WAV;

		} else if (strncmp(ext, "ogg", 3) == 0) { 

			self->type = TYPE_OGG;

		} else {
		
			self->type = TYPE_UNKNOWN;
		
		}

		const char * err = NULL;

		self->stream = (strncmp(type, "stream", 6) == 0) ? true : false;
		
		FILE * file = fopen(filename, "rb");
		
		if (self->type == TYPE_WAV) {

			if (file) {

				bool valid = true;

				char buff[8];

				// Master chunk
				fread(buff, 4, 1, file); // ckId
				if (strncmp(buff, "RIFF", 4) != 0) valid = false;

				fseek(file, 4, SEEK_CUR); // skip ckSize

				fread(buff, 4, 1, file); // WAVEID
				if (strncmp(buff, "WAVE", 4) != 0) valid = false;

				// fmt Chunk
				fread(buff, 4, 1, file); // ckId
				if (strncmp(buff, "fmt ", 4) != 0) valid = false;

				fread(buff, 4, 1, file); // ckSize
				if (*buff != 16) valid = false; // should be 16 for PCM format

				fread(buff, 2, 1, file); // wFormatTag
				if (*buff != 0x0001) valid = false; // PCM format

				u16 channels;
				fread(&channels, 2, 1, file); // nChannels
				self->channels = channels;
				
				u32 rate;
				fread(&rate, 4, 1, file); // nSamplesPerSec
				self->rate = rate;

				fseek(file, 4, SEEK_CUR); // skip nAvgBytesPerSec

				u16 byte_per_block; // 1 block = 1*channelCount samples
				fread(&byte_per_block, 2, 1, file); // nBlockAlign

				u16 byte_per_sample;
				fread(&byte_per_sample, 2, 1, file); // wBitsPerSample
				byte_per_sample /= 8; // bits -> bytes

				// There may be some additionals chunks between fmt and data
				fread(&buff, 4, 1, file); // ckId
				while (strncmp(buff, "data", 4) != 0) {
					u32 size;
					fread(&size, 4, 1, file); // ckSize

					fseek(file, size, SEEK_CUR); // skip chunk

					int i = fread(&buff, 4, 1, file); // next chunk ckId

					if (i < 4) { // reached EOF before finding a data chunk
						valid = false;
						break;
					}
				}

				// data Chunk (ckId already read)
				u32 size;
				fread(&size, 4, 1, file); // ckSize
				self->size = size;

				self->nsamples = self->size / byte_per_block;

				if (byte_per_sample == 1) self->encoding = NDSP_ENCODING_PCM8;
				else if (byte_per_sample == 2) self->encoding = NDSP_ENCODING_PCM16;
				else return "unknown encoding, needs to be PCM8 or PCM16";

				if (!valid) {
					fclose(file);
					return "invalid PCM wav file";
				}

				self->audiochannel = getOpenChannel();
				self->loop = false;

				// Read data
				if (linearSpaceFree() < self->size) return "not enough linear memory available";
				self->data = linearAlloc(self->size);

				fread(self->data, self->size, 1, file);

			} else {

				return "Could not open source, read failure";

			}

			fclose(file);

		} else if (self->type == TYPE_OGG) {

			if (file) {

				if (ov_open(file, &self->vorbisFile, NULL, 0) < 0)
					return "input does not appear to be a valid ogg vorbis file or doesn't exist";

				// Decoding Ogg Vorbis bitstream
				vorbis_info * vorbisInfo = ov_info(&self->vorbisFile, -1);

				if (vorbisInfo == NULL) //No ogg info
					return "could not retrieve ogg audio stream information";

				self->rate = (float)vorbisInfo->rate;

				self->channels = (u32)vorbisInfo->channels;

				self->encoding = NDSP_ENCODING_PCM16;

				self->nsamples = (u32)ov_pcm_total(&self->vorbisFile, -1);

				self->audiochannel = getOpenChannel();

				self->loop = false;

				if (!self->stream) {

					self->size = self->nsamples * self->channels * 2; // *2 because output is PCM16 (2 bytes/sample)

					if (linearSpaceFree() < self->size)
						return "not enough linear memory available";

					self->data = linearAlloc(self->size);

					err = sourceOggDecodeFull(self);
					
					linearFree(&self->vorbisFile);

					ov_clear(&self->vorbisFile);

					fclose(file);
					
				} else {
					self->chunkSamples = fmin(round(0.1 * self->rate), self->nsamples);
					self->size = self->chunkSamples * self->channels * 2; // *2 because output is PCM16 (2 bytes/sample)

					if (linearSpaceFree() < self->size)
						return "not enough linear memory available";

					err = sourceOggDecodeStream(self, false);

					streams[self->audiochannel] = self;
				}

				return err;

			} else {

				return "Could not open source, read failure.";

			}

		} else {
		
			return "Unknown audio type";

		}

	} else {

		return "Could not open source, does not exist";

	}
}

// Decoding loop
const char * sourceOggDecodeFull(love_source * self) {

	int offset = 0;
	int eof = 0;
	int currentSection;

	while (!eof) {
	
		long ret = ov_read(&self->vorbisFile, &self->data[offset], 4096, &currentSection);

		if (ret == 0) {

			eof = 1;
			
		} else if (ret < 0) {
			ov_clear(&self->vorbisFile);

			linearFree(self->data);

			return "error in the ogg vorbis stream";
		} else {
			// TODO handle multiple links (http://xiph.org/vorbis/doc/vorbisfile/decoding.html)

			offset += ret;
		}
	}

	return NULL;
	
}

const char * sourceOggDecodeStream(love_source * self, bool updateDecode) {

	int offset = 0;

	self->data = linearAlloc(self->size);

	while (!self->eof && offset < self->size) {
	
		long ret = ov_read(&self->vorbisFile, &self->data[offset], fmin(self->size - offset, 4096), &self->currentSection);

		if (ret == 0) {

			self->eof = 1;
			
		} else if (ret < 0) {
			ov_clear(&self->vorbisFile);

			linearFree(self->data);

			return "error in the ogg vorbis stream";
		} else {
			offset += ret;
		}
	}

	self->position = ov_raw_tell(&self->vorbisFile);

	if (updateDecode) {
		self->size = offset;
		self->chunkSamples = self->size / self->channels / 2;
		self->nsamples = self->chunkSamples;
	}

	printf("\nPosition: %d\nSize: %d\nSamples: %d, EOF: %d\n", self->position, self->size, self->chunkSamples, self->eof);

	return NULL;

}

int sourceNew(lua_State *L) { // love.audio.newSource()

	if (!soundEnabled) {
		luaError(L, "Could not intialize NDSP.\nPlease dump your dspfirm!");
		return 0;
	}

	const char *filename = luaL_checkstring(L, 1);

	const char * type = luaL_optstring(L, 2, "static");

	love_source *self = luaobj_newudata(L, sizeof(*self));
	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *error = sourceInit(self, filename, type);

	if (error) luaError(L, error);

	return 1;

}

int sourceGC(lua_State *L) { // Garbage Collection

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	ndspChnWaveBufClear(self->audiochannel);

	linearFree(self->data);

	channelList[self->audiochannel] = false;

	return 0;

}

int sourcePlay(lua_State *L) { // source:play()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (self->audiochannel == -1) {
		luaError(L, "No available audio channel");
		return 0;
	}

	ndspChnWaveBufClear(self->audiochannel);
	ndspChnReset(self->audiochannel);
	ndspChnInitParams(self->audiochannel);
	ndspChnSetMix(self->audiochannel, self->mix);
	ndspChnSetInterp(self->audiochannel, self->interp);
	ndspChnSetRate(self->audiochannel, self->rate);
	ndspChnSetFormat(self->audiochannel, NDSP_CHANNELS(self->channels) | NDSP_ENCODING(self->encoding));

	ndspWaveBuf* waveBuf = calloc(1, sizeof(ndspWaveBuf));

	waveBuf->data_vaddr = self->data;
	waveBuf->nsamples = self->nsamples;
	waveBuf->looping = self->loop;

	DSP_FlushDataCache((u32*)self->data, self->size);

	ndspChnWaveBufAdd(self->audiochannel, waveBuf);

	return 0;

}

void sourceStreamPlay(love_source * self) { // source:play()

	if (!soundEnabled) return 0;

	if (self->audiochannel == -1) {
		luaError(L, "No available audio channel");
		return 0;
	}

	ndspChnWaveBufClear(self->audiochannel);
	ndspChnReset(self->audiochannel);
	ndspChnInitParams(self->audiochannel);
	ndspChnSetMix(self->audiochannel, self->mix);
	ndspChnSetInterp(self->audiochannel, self->interp);
	ndspChnSetRate(self->audiochannel, self->rate);
	ndspChnSetFormat(self->audiochannel, NDSP_CHANNELS(self->channels) | NDSP_ENCODING(self->encoding));

	ndspWaveBuf* waveBuf = calloc(1, sizeof(ndspWaveBuf));

	waveBuf->data_vaddr = self->data;
	waveBuf->nsamples = self->nsamples;
	waveBuf->looping = self->loop;

	DSP_FlushDataCache((u32*)self->data, self->size);

	ndspChnWaveBufAdd(self->audiochannel, waveBuf);

	return 0;

}

int sourceStop(lua_State *L) { // source:stop()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	ndspChnWaveBufClear(self->audiochannel);

	return 0;

}

int sourceIsPlaying(lua_State *L) { // source:isPlaying()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushboolean(L, ndspChnIsPlaying(self->audiochannel));

	return 1;

}

int sourceSetLooping(lua_State *L) { // source:setLooping()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	self->loop = lua_toboolean(L, 2);

	return 0;

}

int sourceIsLooping(lua_State *L) { // source:isLooping()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushboolean(L, self->loop);

	return 1;

}

int sourceSetVolume(lua_State *L) { // source:setVolume()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	float vol = luaL_checknumber(L, 2);
	if (vol > 1) vol = 1;
	if (vol < 0) vol = 0;

	for (int i=0; i<=3; i++) self->mix[i] = vol;

	ndspChnSetMix(self->audiochannel, self->mix);

	return 0;

}

int sourceGetVolume(lua_State *L) { // source:getVolume()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->mix[0]);

	return 1;

}

int sourceTell(lua_State *L) { // source:tell()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (!ndspChnIsPlaying(self->audiochannel)) {
		lua_pushnumber(L, 0);
	} else {
		lua_pushnumber(L, (double)(ndspChnGetSamplePos(self->audiochannel)) / self->rate);
	}

	return 1;

}

int sourceGetDuration(lua_State *L) { // source:getDuration()

	if (!soundEnabled) return 0;

	love_source *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, (double)(self->nsamples) / self->rate);

	return 1;

}

int initSourceClass(lua_State *L) {

	luaL_Reg reg[] = {
		{"new",			sourceNew	},
		{"__gc",		sourceGC	},
		{"play",		sourcePlay	},
		{"stop",		sourceStop	},
		{"isPlaying",	sourceIsPlaying},
		{"setLooping",	sourceSetLooping},
		{"isLooping",	sourceIsLooping},
		{"setVolume",	sourceSetVolume},
		{"getVolume",	sourceGetVolume},
		{"tell",		sourceTell},
		{"getDuration", sourceGetDuration},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, sourceNew, reg);

	return 1;

}
