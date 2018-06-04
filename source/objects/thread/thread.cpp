#include "common/runtime.h"
#include "objects/thread/thread.h"

#include "socket/socket.h"
#include "objects/file/file.h"

ThreadClass::ThreadClass(const string & arg)
{
	const char * pathCheck = arg.c_str();
	this->started = false;

	if (LOVE_VALIDATE_FILE_EXISTS_CLEAN(pathCheck))
	{
		File codeFile(pathCheck, "r");
		this->code = codeFile.Read();
	}
	else
		this->code = arg;
}

void ThreadClass::SetError(const string & error)
{
	if (error == "")
		this->error.clear();
	else
		this->error = error;
}

string ThreadClass::GetError()
{
	return this->error;
}

string ThreadClass::GetCode()
{
	return this->code;
}

vector<Variant> ThreadClass::GetArgs()
{
	return this->args;
}

void Run(void * arg)
{
	lua_State * L = luaL_newstate();
	ThreadClass * self = (ThreadClass *)arg;

	//Clear the error string
	self->SetError("");

	//Initialize LOVE for the thread
	luaL_openlibs(L);
	love_preload(L, Socket::Initialize, "socket");
	luaL_requiref(L, "love", Love::Initialize, 1);

	//Joystick::Initialize(L);

	//Get the code to run
	string code = self->GetCode();

	//load our code
	luaL_loadstring(L, code.c_str());

	//get our arguments
	vector<Variant> args = self->GetArgs();

	//pop them into the thread
	for (auto it : args)
		it.ToLua(L);

	//call the code to execute with the args
	//set an error if it occurs
	if (lua_pcall(L, args.size(), 0, 0))
		self->SetError(lua_tostring(L, -1));

	lua_close(L);

	if (!self->GetError().empty())
		self->OnError();
}

void ThreadClass::OnError()
{

}

void ThreadClass::Start(const std::vector<Variant> & args)
{
	this->args = args;
	threadCreate(&this->thread, Run, this, 0x1000, 0x2C, -2);

	threadStart(&this->thread);
	this->started = true;
}

void ThreadClass::Close()
{
	threadClose(&this->thread);
	this->started = false;
}

void ThreadClass::Wait()
{
	threadWaitForExit(&this->thread);
}

bool ThreadClass::IsRunning()
{
	return this->started;
}