#pragma once

namespace IO
{
    enum IO
    {
        IO_DONE = 0,
        IO_TIMEOUT = -1,
        IO_CLOSED = -2,
        IO_UNKNOWN = -3
    };

    inline const char * GetError(int error)
    {
        switch (error)
        {
            case IO_DONE:
                return NULL;
            case IO_CLOSED:
                return "closed";
            case IO_TIMEOUT:
                return "timeout";
            default:
                return "unknown error";
        }
    }
}
