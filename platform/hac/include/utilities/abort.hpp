#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    enum AbortCode
    {
        ABORT_PLU,
        ABORT_NIFM,
        ABORT_ACC,
        ABORT_SET,
        ABORT_SETSYS,
        ABORT_PSM,
        ABORT_SOCKETS,
        ABORT_FRIENDV,
        ABORT_APPLET
    };

    static constexpr const char* ABORT_FORMAT =
        "Failed to initalize %s!\n\n"
        "Result: %ld\n"
        "Description: %ld\n\n"
        "Please visit https://switchbrew.org/wiki/Error_codes for more information.\n";

    static constexpr const char* TITLE_TAKEOVER_ERROR = "Please run LÖVE Potion under "
                                                        "Atmosphère title takeover.";
    // clang-format off
    static constexpr BidirectionalMap abortTypes =
    {
        ABORT_PLU,     "pl:u",
        ABORT_NIFM,    "nifm:u",
        ABORT_ACC,     "acc:a",
        ABORT_SET,     "set",
        ABORT_SETSYS,  "set:sys",
        ABORT_PSM,     "psm",
        ABORT_SOCKETS, "sockets",
        ABORT_FRIENDV, "friend:v"
    };
    // clang-format on
} // namespace love
