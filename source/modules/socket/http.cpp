#include "common/runtime.h"
#include "http.h"

using love::HTTP;

HTTP::HTTP(std::string url)
{
	this->url = url;
}

void HTTP::OpenContext(HTTPC_RequestMethod method, bool disableSSL)
{	
	this->returnResult = httpcOpenContext(&this->httpContext, method, this->url.c_str(), 1);

	if (this->returnResult != 0) 
		console->ThrowError("Failed to request url."); 

	if (disableSSL)
		this->DisableSSL();

	httpcSetKeepAlive(&this->httpContext, HTTPC_KEEPALIVE_ENABLED);
}

void HTTP::DisableSSL()
{
	this->returnResult = httpcSetSSLOpt(&httpContext, SSLCOPT_DisableVerify);

	if (returnResult != 0) 
		console->ThrowError("Failed to disable SSL Verification.");
}

bool HTTP::Redirected()
{
	return ((this->statusCode >= 301 && this->statusCode <= 303) || (this->statusCode >= 307 && this->statusCode <= 308));
}

void HTTP::BeginRequest()
{
	this->returnResult = httpcBeginRequest(&this->httpContext);

	if (returnResult != 0) 
		console->ThrowError("Failed to begin http request");

	this->returnResult = httpcGetResponseStatusCode(&this->httpContext, &this->statusCode);

	char * newURL = nullptr;

	if (this->Redirected())
	{
		if (newURL == nullptr) 
			newURL = (char *)malloc(0x1000); // One 4K page for new URL

		if (newURL == nullptr)
			this->Close();

		httpcGetResponseHeader(&this->httpContext, "Location", newURL, 0x1000);
		this->url = newURL; // Change pointer to the url that we just learned

		httpcCloseContext(&this->httpContext); // Close this context before we try the next
	}
	else
	{
		if (newURL == nullptr && this->statusCode != 200) 
			console->ThrowError("Invalid status code");
	}
}

u32 HTTP::GetStatusCode()
{
	return this->statusCode;
}

bool HTTP::IsDone()
{
	return this->done;
}

void HTTP::AddRequestHeaderField(std::string field, std::string value)
{
	httpcAddRequestHeaderField(&this->httpContext, field.c_str(), value.c_str());
}

u32 HTTP::GetContentSize()
{	
	u32 tempSize = 0;
	httpcGetDownloadSizeState(&this->httpContext, nullptr, &tempSize);
	return tempSize;
}

u32 HTTP::GetSize()
{
	return this->readSize;
}

int HTTP::Download()
{
	this->buffer = (u8 *)malloc(this->bufferSize);
	u32 currentSize = 0;

	do
	{
		this->returnResult = httpcDownloadData(&this->httpContext, this->buffer + currentSize, this->bufferSize, &this->size);
		currentSize += this->size;

		if (this->returnResult == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING)
		{
			this->buffer = (u8 *)realloc(this->buffer, currentSize + this->bufferSize);
		
			if (this->buffer == NULL)
				console->ThrowError("Buffer failed to resize");
		}
	} while (this->returnResult == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);

	if (this->returnResult != 0) 
		console->ThrowError("Data failed to download.");

	this->buffer = (u8 *)realloc(this->buffer, currentSize);
	this->readSize = currentSize;

	this->done = true;

	return 0;
}

u8 * HTTP::GetBuffer()
{
	return this->buffer;
}

void HTTP::Close()
{
	httpcCloseContext(&this->httpContext);
}