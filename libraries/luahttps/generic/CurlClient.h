#pragma once

#include <curl/curl.h>

#include "../common/HTTPSClient.h"

class CurlClient : public HTTPSClient
{
  public:
    virtual bool valid() const override;
    virtual HTTPSClient::Reply request(const HTTPSClient::Request& req) override;
};
