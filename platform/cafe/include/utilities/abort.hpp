#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    enum AbortCode
    {
        ABORT_AC,
        ABORT_BSP
    };

    static constexpr const char* ABORT_TITLE = "Failed to initalize %s!";

    static constexpr const char* ABORT_FORMAT =
        "Result: %ld\n"
        "Please visit https://wiiubrew.org/wiki/Error_codes for more information.\n";

    // clang-format off
    static constexpr BidirectionalMap abortTypes = {
        ABORT_AC, "nn:ac",
        ABORT_AC, "bsp"
    };
    // clang-format on
} // namespace love
