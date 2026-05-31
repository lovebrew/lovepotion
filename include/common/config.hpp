#pragma once

namespace love
{
    namespace detail
    {
        template<typename... T>
        char unused(T&&...);
    } // namespace detail
} // namespace love

#define LOVE_UNUSED(...) (void)sizeof(love::detail::unused(__VA_ARGS__))
