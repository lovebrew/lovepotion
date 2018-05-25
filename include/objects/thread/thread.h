#pragma once

class ThreadClass
{
	public:
		ThreadClass(const std::string & arg);
		
		bool IsRunning();
		void Start();
		void Wait();
		void Close();

		void Collect();

		std::string GetCode();

		void SetError(const std::string & error);
		std::string GetError();

	private:
		std::string arg;
		bool started;
		Thread thread;
		std::string error;
};

extern void Run(void * arg);