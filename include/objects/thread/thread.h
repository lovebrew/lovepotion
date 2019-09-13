#pragma once

class LuaThread : public Object
{
    public:
        LuaThread(const std::string & code);

        bool IsRunning();
        bool Start(const std::vector<Variant> & args);
        void Wait();
        void Close();

        void Collect();

        std::string GetCode();
        std::vector<Variant> GetArgs();

        void SetError(const std::string & error);
        std::string GetError();
        void OnError();
        void Run();

    private:
        std::string code;
        std::vector<Variant> args;

        bool started;
        Thread thread;
        std::string error;

};

extern void TestRun(void * arg);
