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

#include "shared.h"

void combine(char* destination, const char* path1, const char* path2) {
	
	if(path1 == NULL && path2 == NULL) {
		strcpy(destination, "");;
	}
	else if(path2 == NULL || strlen(path2) == 0) {
		strcpy(destination, path1);
	}
	else if(path1 == NULL || strlen(path1) == 0) {
		strcpy(destination, path2);
	} 
	else {
		char directory_separator[] = "/";
		const char *last_char = path1;
		while(*last_char != '\0')
			last_char++;        
		int append_directory_separator = 0;
		if(strcmp(last_char, directory_separator) != 0) {
			append_directory_separator = 1;
		}
		strcpy(destination, path1);
		if(append_directory_separator)
			strcat(destination, directory_separator);
		strcat(destination, path2);
	}
}

int fileExists (char *filename){
	struct stat st;
	int result = stat(filename, &st);
	return result == 0;
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
	
	char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator

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