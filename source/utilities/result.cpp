#include <utilities/result.hpp>

using namespace love;

ResultCode::ResultCode(int32_t value)
{
    this->result = value;
}

bool ResultCode::Success() const
{
    return this->result >= 0;
}

bool ResultCode::Failed() const
{
    return this->result < 0;
}
