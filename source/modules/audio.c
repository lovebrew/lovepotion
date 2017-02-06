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

bool soundEnabled;

static int audioStop(lua_State *L) { // love.audio.stop()

	if (!soundEnabled) return 0;

	for (int i = 0; i <= 23; i++) {
		ndspChnWaveBufClear(i);
	}

	return 0;

}

static int audioSetVolume(lua_State *L) { // love.audio.setVolume()

	float vol = luaL_checknumber(L, 1);
	if (vol > 1) vol = 1;
	if (vol < 0) vol = 0;

	float mix[12];

	for (int i=0; i<=3; i++) mix[i] = vol;
	for (int i=0; i<=23; i++) ndspChnSetMix(i, mix);

	return 0;
	
}

int sourceNew(lua_State *L);

int initLoveAudio(lua_State *L) {

	soundEnabled = !ndspInit();

	luaL_Reg reg[] = {
		{ "stop",		audioStop	},
		{ "newSource",	sourceNew	},
		{ "setVolume", audioSetVolume},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}