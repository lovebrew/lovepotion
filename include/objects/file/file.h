#pragma once

class File : public Object
{
	public:
		File(const char * path);
		File(const char * path, const char * mode);

		File() {};
		~File();

		char * Read();
		u8 * ReadBinary();

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