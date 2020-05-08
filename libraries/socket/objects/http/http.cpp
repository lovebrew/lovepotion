#include "common/runtime.h"
#include "socket/objects/http/http.h"

#include <curl/curl.h>

static size_t WriteMemoryCallback(void * contents, size_t size, size_t nmemb, void * userdata)
{
    std::vector<char> * recv = (std::vector<char> *)userdata;
    recv->insert(recv->end(), (char *)contents, (char *)contents + size * nmemb);

    return size * nmemb;
}

static size_t HeaderCallback(char * buffer, size_t size, size_t nitems, void * userdata)
{
    std::vector<HTTP::HTTPHeader> * headers = (std::vector<HTTP::HTTPHeader> *)userdata;
    HTTP::HTTPHeader header;

    std::string bufferString(buffer, nitems);

    size_t tmp = bufferString.find_first_of(":");

    header.key = bufferString.substr(0, tmp);
    std::string value = bufferString.substr(tmp + 1);

    size_t first = value.find_first_not_of(" \t\r\n");
    size_t last = value.find_last_not_of(" \t\r\n");

    header.value = value.substr(first, (last - first) + 1);

    headers->push_back(header);

    return nitems * size;
}

HTTP::Response HTTP::Request(const HTTP::Options & options)
{
    HTTP::Response ret = HTTP::Response();
    struct curl_slist * headers = NULL;

    CURL * curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, options.url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "LovePotion/2.0.0");

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, options.redirect);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

    if (!options.headers.empty())
    {
        for (size_t index = 0; index < options.headers.size(); index++)
        {
            std::string value = options.headers[index].key + ":" + options.headers[index].value;
            headers = curl_slist_append(headers, value.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    if (options.method == "GET")
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    else if (options.method == "HEAD")
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&ret.headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&ret.buffer);

    CURLcode res = curl_easy_perform(curl);

    if(res != CURLE_OK)
        LOG("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &ret.code);

    curl_easy_cleanup(curl);

    return ret;
}
