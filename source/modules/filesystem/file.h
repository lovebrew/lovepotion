#pragma once

namespace love
{
	class File
	{
		public:
			const char * Init(const char * name);
			const char * InitPath(const char * path);

			void Write(void * data, int len);
			void Flush();
			void Close();
			char * Read();
			bool Open(const char * mode);
			int GetSize();
			bool IsOpen();
			const char * GetMode();
		
		private:
			const char * fileName;
			const char * mode;
			bool open;
			FILE * fileHandle;
			
	};
}