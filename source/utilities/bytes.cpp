#include <common/exception.hpp>

#include <utilities/bytes.hpp>

uint8_t love::Nibble(char c)
{
    if (c >= '0' && c <= '9')
        return (uint8_t)(c - '0');

    if (c >= 'A' && c <= 'F')
        return (uint8_t)(c - 'A' + 0x0A);

    if (c >= 'a' && c <= 'f')
        return (uint8_t)(c - 'a' + 0x0A);

    return 0;
}

char* love::BytesToHex(const uint8_t* source, size_t sourceLength,
                       size_t& destinationLength)
{
    destinationLength = sourceLength * 2;

    if (destinationLength == 0)
        return nullptr;

    char* destination = nullptr;

    try
    {
        destination = new char[destinationLength + 1];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    for (size_t i = 0; i < sourceLength; i++)
    {
        uint8_t b = source[i];

        destination[i * 2 + 0] = hexChars[b >> 0x04];
        destination[i * 2 + 1] = hexChars[b & 0x0F];
    }

    destination[destinationLength] = '\0';

    return destination;
}

uint8_t* love::HexToBytes(const char* source, size_t sourceLength,
                          size_t& destinationLength)
{
    if (sourceLength >= 2 && source[0] == '0' && (source[1] == 'x' || source[1] == 'X'))
    {
        source += 2;
        sourceLength -= 2;
    }

    destinationLength = (sourceLength + 1) / 2;

    if (destinationLength == 0)
        return nullptr;

    uint8_t* destination = nullptr;

    try
    {
        destination = new uint8_t[destinationLength];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    for (size_t i = 0; i < destinationLength; i++)
    {
        destination[i] = Nibble(source[i * 2]) << 4;

        if (i * 2 + 1 < sourceLength)
            destination[i] |= Nibble(source[i * 2 + 1]);
    }

    return destination;
}
