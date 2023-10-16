#pragma once

#include <utilities/hashfunction/hashfunction.hpp>

namespace love
{
    class MD5 : public HashFunction
    {
      public:
        bool IsSupported(Function function) const override
        {
            return function == FUNCTION_MD5;
        };

        void Hash(Function function, const char* input, uint64_t length,
                  Value& output) const override;

      private:
        static constexpr uint8_t shifts[0x40] = {
            0x07, 0x0C, 0x11, 0x16, 0x07, 0x0C, 0x11, 0x16, 0x07, 0x0C, 0x11, 0x16, 0x07,
            0x0C, 0x11, 0x16, 0x05, 0x09, 0x0E, 0x14, 0x05, 0x09, 0x0E, 0x14, 0x05, 0x09,
            0x0E, 0x14, 0x05, 0x09, 0x0E, 0x14, 0x04, 0x0B, 0x10, 0x17, 0x04, 0x0B, 0x10,
            0x17, 0x04, 0x0B, 0x10, 0x17, 0x04, 0x0B, 0x10, 0x17, 0x06, 0x0A, 0x0F, 0x15,
            0x06, 0x0A, 0x0F, 0x15, 0x06, 0x0A, 0x0F, 0x15, 0x06, 0x0A, 0x0F, 0x15
        };

        static constexpr uint32_t constants[0x40] = {
            0XD76AA478, 0XE8C7B756, 0X242070DB, 0XC1BDCEEE, 0XF57C0FAF, 0X4787C62A,
            0XA8304613, 0XFD469501, 0X698098D8, 0X8B44F7AF, 0XFFFF5BB1, 0X895CD7BE,
            0X6B901122, 0XFD987193, 0XA679438E, 0X49B40821, 0XF61E2562, 0XC040B340,
            0X265E5A51, 0XE9B6C7AA, 0XD62F105D, 0X02441453, 0XD8A1E681, 0XE7D3FBC8,
            0X21E1CDE6, 0XC33707D6, 0XF4D50D87, 0X455A14ED, 0XA9E3E905, 0XFCEFA3F8,
            0X676F02D9, 0X8D2A4C8A, 0XFFFA3942, 0X8771F681, 0X6D9D6122, 0XFDE5380C,
            0XA4BEEA44, 0X4BDECFA9, 0XF6BB4B60, 0XBEBFBC70, 0X289B7EC6, 0XEAA127FA,
            0XD4EF3085, 0X04881D05, 0XD9D4D039, 0XE6DB99E5, 0X1FA27CF8, 0XC4AC5665,
            0XF4292244, 0X432AFF97, 0XAB9423A7, 0XFC93A039, 0X655B59C3, 0X8F0CCC92,
            0XFFEFF47D, 0X85845DD1, 0X6FA87E4F, 0XFE2CE6E0, 0XA3014314, 0X4E0811A1,
            0XF7537E82, 0XBD3AF235, 0X2AD7D2BB, 0XEB86D391
        };
    };
} // namespace love
