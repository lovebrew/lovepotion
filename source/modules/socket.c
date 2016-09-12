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

bool initializeSocket;
static void * socketBuffer;

int newUDP(lua_State * L);
int socketNewTCP(lua_State * L);

int socketShutdown(lua_State * L) {
	if (initializeSocket) {
		socExit();
		
		initializeSocket = false;
	}

	return 0;
}

int initSocket(lua_State *L) {
	if (!initializeSocket) {
		socketBuffer = memalign(0x1000, 0x100000);

		Result socketIsInitialized = socInit(socketBuffer, 0x100000);

		if (R_FAILED(socketIsInitialized)) {
			luaError(L, "Failed to initialize LuaSocket.");
		} else {
			initializeSocket = true;
		}
	}

	luaL_Reg reg[] = {
		{"udp", newUDP},
		{"tcp", socketNewTCP},
		{"shutdown", socketShutdown},
		{ 0, 0 },
	};

	luaL_newmetatable(L, "Socket");
	
	lua_pushvalue(L, -1);
	
	lua_setfield(L, -2, "__index");

	luaL_newlib(L, reg);

	return 1;
}