#include "common/runtime.h"
#include "modules/mod_thread.h"

#include "objects/thread/thread.h"

#define CLASS_TYPE LUAOBJ_TYPE_THREAD
#define CLASS_NAME "Thread"

int threadNew(lua_State * L)
{
	const char * arg = luaL_checkstring(L, 1);

	void * raw_self = luaobj_newudata(L, sizeof(ThreadClass));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	ThreadClass * thread = new (raw_self) ThreadClass(arg);

	return 1;
}

int threadStart(lua_State * L)
{
	ThreadClass * self = (ThreadClass *)luaobj_checkudata(L, 1, CLASS_TYPE);

	if (!self->IsRunning())
		self->Start();

	return 0;
}

int initThreadClass(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "new", threadNew },
		{ 0, 0 }
	};

	luaobj_newclass(L, CLASS_NAME, NULL, threadNew, reg);

	return 1;
}