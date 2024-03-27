#pragma once

#include "common/Object.hpp"
#include "common/Variant.hpp"

#include <vector>

namespace love
{
    class Message : public Object
    {
      public:
        Message(const std::string& name, const std::vector<Variant>& args = {});

        virtual ~Message();

        std::string name;
        std::vector<Variant> args;
    };
} // namespace love
