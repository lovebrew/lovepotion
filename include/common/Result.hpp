#pragma once

#if defined(__3DS__)
    #include <3ds/result.h>
#elif defined(__SWITCH__)
    #include <switch/result.h>
#elif defined(__WIIU__)
    /* https://github.com/devkitPro/wut/blob/master/include/nn/result.h */
    #define R_SUCCEEDED(result) ((result) >= 0)
#else
    #error "Invalid platform"
#endif

#include <concepts>
#include <cstdint>

template<typename NNResult>
concept NNResultConcept = requires(NNResult result) {
    { result.value } -> std::convertible_to<int32_t>;
};

class ResultCode
{
  public:
    static constexpr int32_t DSP_FIRM_MISSING = 0xD880A7FA;

    ResultCode() : code(0)
    {}

    explicit constexpr ResultCode(int32_t result) : code(result)
    {}

    explicit constexpr ResultCode(uint32_t result) : code(result)
    {}

    template<NNResultConcept NNResult>
    explicit constexpr ResultCode(NNResult result) : code(result.value)
    {}

    constexpr bool operator==(int32_t other) const
    {
        return this->code == other;
    }

    explicit constexpr operator bool() const
    {
        return R_SUCCEEDED(this->code);
    }

    constexpr operator int32_t() const
    {
        return this->code;
    }

    constexpr int32_t value() const
    {
        return this->code;
    }

  private:
    int32_t code;
};
