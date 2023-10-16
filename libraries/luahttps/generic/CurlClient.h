#pragma once

#include "../common/config.h"

#ifdef HTTPS_BACKEND_CURL

#include <curl/curl.h>

#include "../common/HTTPSClient.h"

class CurlClient : public HTTPSClient
{
public:
	virtual bool valid() const override;
	virtual HTTPSClient::Reply request(const HTTPSClient::Request &req) override;

private:
	static struct Curl
	{
		Curl();
		~Curl();
		void *handle;
		bool loaded;
	} curl;
};

#endif // HTTPS_BACKEND_CURL
