#pragma once

#if defined(HTTPS_HAVE_CONFIG_GENERATED_H)
    #include "common/config-generated.h"
#elif defined(WIN32) || defined(_WIN32)
    #define HTTPS_BACKEND_SCHANNEL
    #define HTTPS_USE_WINSOCK
    #include <winapifamily.h>
    #if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
        // WinINet is only supported on desktop.
        #define HTTPS_BACKEND_WININET
    #endif
#elif defined(__ANDROID__)
    #define HTTPS_BACKEND_ANDROID
#elif defined(__APPLE__)
    #define HTTPS_BACKEND_NSURL
#elif defined(__3DS__) || defined(__SWITCH__) || defined(__WIIU__)
    #define HTTPS_BACKEND_CURL
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #if defined __has_include
        #if __has_include(<curl/curl.h>)
            #define HTTPS_BACKEND_CURL
        #endif
        #if __has_include(<openssl/ssl.h>)
            #define HTTPS_BACKEND_OPENSSL
        #endif
    #else
        // Hope for the best...
        #define HTTPS_BACKEND_CURL
        #define HTTPS_BACKEND_OPENSSL
    #endif
#endif
