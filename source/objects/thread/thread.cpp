#include "common/runtime.h"
#include "objects/thread/thread.h"

#include "objects/file/file.h"

ThreadClass::ThreadClass(const string & arg)
{
	const char * pathCheck = arg.c_str();

	if (LOVE_VALIDATE_FILE_EXISTS_CLEAN(pathCheck))
	{
		File codeFile(pathCheck, "r");
		this->arg = codeFile.Read();
	}
	else
		this->arg = arg;
}

void ThreadClass::SetError(const string & error)
{
	this->error = error;
}

string ThreadClass::GetError()
{
	return this->error;
}

string ThreadClass::GetCode()
{
	return this->arg;
}

void Run(void * arg)
{
	lua_State * threadState = luaL_newstate();
	ThreadClass * self = (ThreadClass *)arg;
	string code = self->GetCode();

	if (luaL_dostring(threadState, code.c_str()))
	{
		self->SetError(lua_tostring(threadState, -1));
		self->Close();
		self->Wait();
	}
}

void ThreadClass::Start()
{
	threadCreate(&this->thread, Run, this, 0x1000, 0x2C, -2);

	threadStart(&this->thread);
	this->started = true;
}

void ThreadClass::Close()
{
	threadClose(&this->thread);
}

void ThreadClass::Wait()
{
	threadWaitForExit(&this->thread);
}

bool ThreadClass::IsRunning()
{
	return this->started;
}