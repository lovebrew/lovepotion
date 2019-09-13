#include "common/runtime.h"
#include "objects/thread/thread.h"

#include "socket/luasocket.h"
#include "objects/file/file.h"

LuaThread::LuaThread(const string & arg) : Object("Thread")
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

void LuaThread::SetError(const string & error)
{
    if (error == "")
        this->error.clear();
    else
        this->error = error;
}

string LuaThread::GetError()
{
    return this->error;
}

string LuaThread::GetCode()
{
    return this->code;
}

vector<Variant> LuaThread::GetArgs()
{
    return this->args;
}

void LuaThread::Run()
{
    lua_State * L = luaL_newstate();

    //Clear the error string
    this->SetError("");

    //Initialize LOVE for the thread
    luaL_openlibs(L);

    //love_preload(L, LuaSocket::Initialize, "socket");
    //luaL_requiref(L, "love", Love::Initialize, 1);

    //Joystick::Initialize(L);

    //Get the code to run
    string code = this->GetCode();
    
    //get our arguments
    vector<Variant> args = this->GetArgs();

    //load our code
    const char * codeBuffer = code.c_str();
    size_t length = code.size();

    if (luaL_loadbuffer(L, codeBuffer, length, this->ToString()) != 0)
        this->SetError(lua_tostring(L, -1));
    else
    {
        uint numargs = args.size();

        //pop args onto the function
        for (uint i = 0; i < numargs; i++)
            args[i].ToLua(L);

        args.clear();

        //call the code to execute with the args
        //set an error if it occurs
        if (lua_pcall(L, numargs, 0, 0) != 0)
            this->SetError(lua_tostring(L, -1));
    }

    //LuaSocket::Close();
    lua_close(L);

    if (!this->GetError().empty())
        this->OnError();
}

void LuaThread::OnError()
{

}

bool LuaThread::IsRunning()
{
    return this->started;
}
