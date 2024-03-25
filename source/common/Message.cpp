#include "common/Message.hpp"

namespace love
{
    Message::Message(const std::string& name, const std::vector<Variant>& args) :
        name(name),
        args(args)
    {}

    Message::~Message()
    {}
} // namespace love
