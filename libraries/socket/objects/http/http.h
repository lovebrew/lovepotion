#pragma once

class HTTP
{
    public:
        struct HTTPHeader
        {
            std::string key;
            std::string value;
        };

        struct Response
        {
            std::vector<char> buffer;
            long code;
            std::vector<HTTPHeader> headers;
        };

        struct Options
        {
            std::string url;
            std::string method;
            std::vector<HTTPHeader> headers;
            bool redirect;
        };

        HTTP::Response Request(const HTTP::Options & options);
};
