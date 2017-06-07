#pragma once

namespace love
{
	class File
	{
		public:
			char * Init(const char * name);
			char * InitPath(const char * path);

			void Write(const char * data);
			void Flush();
			void Close();
			const char * Read();
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