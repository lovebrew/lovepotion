#include "HTTPSCommon.h"
#include "ConnectionClient.h"

#include <stdexcept>

#include "../generic/CurlClient.h"
static CurlClient curlclient;

static HTTPSClient* clients[] = {
    &curlclient,
    nullptr,
};

HTTPSClient::Reply request(const HTTPSClient::Request& req)
{
    for (size_t i = 0; clients[i]; ++i)
    {
        HTTPSClient& client = *clients[i];

        if (client.valid())
            return client.request(req);
    }

    throw std::runtime_error("No applicable HTTPS implementation found");
}
