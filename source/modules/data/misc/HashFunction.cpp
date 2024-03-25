#include "modules/data/misc/HashFunction.hpp"

#include "modules/data/misc/MD5.hpp"
#include "modules/data/misc/SHA1.hpp"
#include "modules/data/misc/SHA256.hpp"
#include "modules/data/misc/SHA512.hpp"

namespace love
{
    HashFunction* HashFunction::getHashFunction(Function function)
    {
        switch (function)
        {
            case FUNCTION_MD5:
                return &md5;
            case FUNCTION_SHA1:
                return &sha1;
            case FUNCTION_SHA224:
            case FUNCTION_SHA256:
                return &sha256;
            case FUNCTION_SHA384:
            case FUNCTION_SHA512:
                return &sha512;
            case FUNCTION_MAX_ENUM:
                return nullptr;
                // No default for compiler warnings
        }

        return nullptr;
    }
} // namespace love
