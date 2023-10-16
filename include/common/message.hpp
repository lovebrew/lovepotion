#pragma once

#include <common/object.hpp>
#include <common/variant.hpp>

#include <string>
#include <vector>

namespace love
{
    class Message : public Object
    {
      public:
        Message(const std::string& name, const std::vector<Variant>& args = {}) :
            name(name),
            args(args)
        {}

        ~Message()
        {}

        const std::string name;
        const std::vector<Variant> args;
    };
} // namespace love
