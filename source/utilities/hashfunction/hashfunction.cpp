#include <utilities/bidirectionalmap.hpp>

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

// clang-format off
constexpr auto functionNames = BidirectionalMap<>::Create(
    "md5",    HashFunction::Function::FUNCTION_MD5,
    "sha1",   HashFunction::Function::FUNCTION_SHA1,
    "sha224", HashFunction::Function::FUNCTION_SHA224,
    "sha256", HashFunction::Function::FUNCTION_SHA256,
    "sha384", HashFunction::Function::FUNCTION_SHA384,
    "sha512", HashFunction::Function::FUNCTION_SHA512
);
// clang-format on

bool HashFunction::GetConstant(const char* in, Function& out)
{
    return functionNames.Find(in, out);
}

bool HashFunction::GetConstant(const Function& in, const char*& out)
{
    return functionNames.ReverseFind(in, out);
}
std::vector<const char*> HashFunction::GetConstants(Function)
{
    return functionNames.GetNames();
}
