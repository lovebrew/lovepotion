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

#define CONFIG_3D_SLIDERSTATE (*(float*)0x1FF81080)

int fileExists(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (file) fclose(file);
	return (file);
}

void luaError(lua_State *L, char *message) {
	luaL_error(L, message);
}

const char *fileExtension(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if(!dot || dot == filename) return "";
	return dot + 1;
}

char* concat(char *s1, char *s2) {
	
	char *result = malloc(strlen(s1)+strlen(s2)+1); // +1 for the zero-terminator

	strcpy(result, s1);
	strcat(result, s2);

	return result;
}

int getType(const char *name) {

	const char *ext = fileExtension(name);

	if (strncmp(ext, "png", 3) == 0) {
		return 0;
	} else if (strncmp(ext, "jpeg", 4) == 0 || strncmp(ext, "jpg", 3) == 0) {
		return 1;
	} else if (strncmp(ext, "bmp", 3) == 0) {
		return 2;
	} else {
		return 4;
	}

}