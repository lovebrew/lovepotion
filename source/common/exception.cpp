#include "common/exception.h"

#include <iostream>

using namespace love;

Exception::Exception(const char* format, ...)
{
    va_list args;

    int sizeBuffer = 256;
    int sizeOut    = 0;
    char* buffer;

    while (true)
    {
        buffer = new char[sizeBuffer];
        memset(buffer, 0, sizeBuffer);

        va_start(args, format);
        sizeOut = vsnprintf(buffer, sizeBuffer, format, args);
        va_end(args);

        if (sizeOut == sizeBuffer || sizeOut == -1 || sizeOut == sizeBuffer - 1)
            sizeBuffer *= 2;
        else if (sizeOut > sizeBuffer)
            sizeBuffer = sizeOut + 2;
        else
            break;

        delete[] buffer;
    }

    this->message = std::string(buffer);
    delete[] buffer;
}

Exception::~Exception() throw()
{}
