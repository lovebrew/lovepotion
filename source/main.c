extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <libtransistor/nx.h>
#include <stdio.h>

#define ASSERT_OK(label, expr) if((r = expr) != RESULT_OK) {            \
		printf("assertion failed at %s:%d: result 0x%x is not OK\n", __FILE__, __LINE__, r); \
		goto label; \
}

bool LUA_ERROR = false;
lua_State * L;

int main(int argc, char ** argv)
{
	L = luaL_newstate();

	luaL_dostring(L, "jit.off()");

	luaL_openlibs(L);
	
	result_t r;
	ASSERT_OK(fail, sm_init());
	ASSERT_OK(fail_sm, hid_init());

	printf("LOVE2D 0.10.2 for Switch!");

	/*
	loveChangeDir(FUSED);

	luaL_requiref(L, "love", loveInit, 1);

	if (luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot"))
		throwError(L);

	loveCreateSaveDirectory();

	osSetSpeedupEnable(true);
	
	s32 priority = 0;
	svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
	Thread musicThread = threadCreate(sourceStream, NULL, 0x1000, priority - 1, -2, false);

	if (luaL_dostring(L, "if love.load then love.load() end"))
		console->ThrowError(L);
	*/

	while (true)
	{
		if (LOVE_QUIT)
			break;
	}

	/*updateAudioThread = false;

	loveClose(L);

	threadJoin(musicThread, U64_MAX);
	threadFree(musicThread);
	
	if (FUSED) 
		romfsExit();
	*/
	return 0;
}