#pragma once

#include "common/version.h"
#include "modules/filesystem/file.h"

namespace love
{
	class HTTP
	{
		public:
			HTTP(std::string url);
		
			void OpenContext(HTTPC_RequestMethod method, bool disableSSL);
			
			void DisableSSL();
			
			void BeginRequest();
			
			void AddRequestHeaderField(std::string field, std::string value);

			u32 GetContentSize();
			u32 GetStatusCode();

			int Download();
			u8 * GetBuffer();

			bool Redirected();
			bool IsDone();
			void Close();
			u32 GetSize();

		private:
			httpcContext httpContext;
			Result returnResult = 0;
			u8 * buffer;
			u32 statusCode;
			u32 size = 0;
			u32 readSize = 0;
			u32 bufferSize = 1024;
			
			std::string url;
			std::string destination;
			std::string filename;

			bool done = false;
	};
}