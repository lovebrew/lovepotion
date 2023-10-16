#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/hashfunction/hashfunction.hpp>

#include <utilities/hashfunction/types/md5.hpp>
#include <utilities/hashfunction/types/sha1.hpp>
#include <utilities/hashfunction/types/sha256.hpp>
#include <utilities/hashfunction/types/sha512.hpp>

using namespace love;

HashFunction* HashFunction::GetHashFunction(Function function)
{
    switch (function)
    {
        case FUNCTION_MD5:
        {
            static MD5 md5;
            return &md5;
        }
        case FUNCTION_SHA1:
        {
            static SHA1 sha1;
            return &sha1;
        }
        case FUNCTION_SHA224:
        case FUNCTION_SHA256:
        {
            static SHA256 sha256;
            return &sha256;
        }
        case FUNCTION_SHA384:
        case FUNCTION_SHA512:
        {
            static SHA512 sha512;
            return &sha512;
        }
        case FUNCTION_MAX_ENUM:
            return nullptr;
    }

    return nullptr;
}
