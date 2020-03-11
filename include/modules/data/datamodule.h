#pragma once

#include "common/stringmap.h"

namespace love
{
    namespace data
    {
        enum EncodeFormat
        {
            ENCODE_BASE64,
            ENCODE_HEX,
            ENCODE_MAX_ENUM
        };

        enum ContainerType
        {
            CONTAINER_DATA,
            CONTAINER_STRING,
            CONTAINER_MAX_ENUM
        };
    }

    class DataModule : public Module
    {
        public:
            static bool GetConstant(const char * in, love::data::ContainerType & out);
            static std::vector<std::string> GetConstants();

            ModuleType GetModuleType() const { return M_DATA; }

            const char * GetName() const override { return "love.data"; }
    };
}
