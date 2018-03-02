#pragma once

class File
{
	public:
		File(const char * path);
		File() {};

		char * Read();
		void Write(const char * data, size_t length);

		const char * GetMode();

		bool Open(const char * mode);
		bool IsOpen();
		void Flush();
		void Close();

		long GetSize();
	
	private:
		const char * path;
		const char * mode;

		FILE * fileHandle;
		bool open;
};