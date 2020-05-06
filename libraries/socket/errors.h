#pragma once

/* Socket Errors */

#define SOC_ERROR_HOST_NOT_FOUND       "host not found"
#define SOC_ERROR_ADDRESS_IN_USE       "address already in use"
#define SOC_ERROR_IS_CONNECTED         "already connected"
#define SOC_ERROR_PERMISSION_DENIED    "permission denied"
#define SOC_ERROR_CONNECTION_REFUSED   "connection refused"
#define SOC_ERROR_CONNECTION_ABORTED   "closed"
#define SOC_ERROR_CONNECTION_RESET     (SOC_ERROR_CONNECTION_ABORTED)
#define SOC_ERROR_CONNECTION_TIMEOUT   "timeout"
#define SOC_ERROR_TEMP_NAME_FAILURE    "temporary failure in name resolution"
#define SOC_ERROR_BAD_FLAGS            "invalid value for ai_flags"
#define SOC_ERROR_BAD_HINTS            "invalid value for hints"
#define SOC_ERROR_FATAL_NAME_FAILURE   "non-recoverable failure in name resolution"
#define SOC_ERROR_FAMILY_NOT_SUPPORTED "ai_family not supported"
#define SOC_ERROR_MEMORY_ALLOCATION    "memory allocation failure"
#define SOC_ERROR_NO_NAME              "host or service not provided, or not known"
#define SOC_ERROR_BUFFER_OVERFLOW      "argument buffer overflow"
#define SOC_ERROR_UNKNOWN_PROTOCOL     "resolved protocol is unknown"
#define SOC_ERROR_UNSUPPORTED_SERVICE  "service not supported for socket type"
#define SOC_ERROR_UNSUPPORTED_SOCKTYPE "ai_socktype not supported"
