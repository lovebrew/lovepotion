#include "CurlClient.h"

#include <mutex>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace
{
    std::once_flag once;
}

static size_t stringstreamWriter(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    std::stringstream* ss = (std::stringstream*)userdata;

    size_t count = size * nmemb;
    ss->write(ptr, count);

    return count;
}

static size_t headerWriter(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    std::map<std::string, std::string>& headers = *((std::map<std::string, std::string>*)userdata);

    size_t count = size * nmemb;
    std::string line(ptr, count);

    size_t split   = line.find(':');
    size_t newline = line.find('\r');

    if (newline == std::string::npos)
        newline = line.size();

    if (split != std::string::npos)
        headers[line.substr(0, split)] = line.substr(split + 1, newline - split - 1);

    return count;
}

bool CurlClient::valid() const
{
    std::call_once(once, curl_global_init, CURL_GLOBAL_DEFAULT);

    return true;
}

HTTPSClient::Reply CurlClient::request(const HTTPSClient::Request& req)
{
    std::call_once(once, curl_global_init, CURL_GLOBAL_DEFAULT);

    Reply reply {};
    reply.responseCode = 400;

    CURL* handle = curl_easy_init();

    if (!handle)
        throw std::runtime_error("Could not create curl request");

    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(handle, CURLOPT_URL, req.url.c_str());
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);

    if (req.method == "PUT")
        curl_easy_setopt(handle, CURLOPT_PUT, 1L);
    if (req.method == "POST")
    {
        curl_easy_setopt(handle, CURLOPT_POST, 1L);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req.postdata.c_str());
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, req.postdata.size());
    }
    else
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, req.method.c_str());

    if (req.postdata.size() > 0 && (req.method != "GET" && req.method != "HEAD"))
    {
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req.postdata.c_str());
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, req.postdata.size());
    }

    // Curl doesn't copy memory, keep the strings around
    std::vector<std::string> lines;
    for (auto& header : req.headers)
    {
        std::stringstream line;
        line << header.first << ": " << header.second;
        lines.push_back(line.str());
    }

    curl_slist* sendHeaders = nullptr;
    for (auto& line : lines)
        sendHeaders = curl_slist_append(sendHeaders, line.c_str());

    if (sendHeaders)
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, sendHeaders);

    std::stringstream body;

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, stringstreamWriter);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &body);

    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerWriter);
    curl_easy_setopt(handle, CURLOPT_HEADERDATA, &reply.headers);

    curl_easy_perform(handle);

    if (sendHeaders)
        curl_slist_free_all(sendHeaders);

    {
        long responseCode;
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &responseCode);
        reply.responseCode = (int)responseCode;
    }

    reply.body = body.str();

    curl_easy_cleanup(handle);

    return reply;
}
