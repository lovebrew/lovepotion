#include "common/runtime.h"
#include "modules/mod_thread.h"

#include "objects/thread/thread.h"
#include "objects/thread/wrap_thread.h"

int LoveThread::NewChannel(lua_State * L)
{
	return 0;
}

void LoveThread::Exit()
{
	//for (auto thread : threads)
	//	thread->Collect();
}

int LoveThread::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "newThread",	threadNew  },
		{ "newChannel", NewChannel },
		{ 0, 0 }
	};
	
	luaL_newlib(L, reg);

	return 1;
}