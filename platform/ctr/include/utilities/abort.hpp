#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    enum AbortCode
    {
        ABORT_MCU_HWC,
        ABORT_PTMU,
        ABORT_CFGU,
        ABORT_AC,
        ABORT_FRD,
        ABORT_SOC,
        ABORT_Y2R,
        ABORT_HID_ACCEL,
        ABORT_HID_GYRO
    };

    static constexpr const char* ABORT_FORMAT_KNOWN =
        "Failed to initalize %s!\n\n"
        "Result: %ld\n"
        "Level: %ld\n"
        "Summary: %ld\n"
        "Description: %ld\n\n"
        "Please visit https://www.3dbrew.org/wiki/Error_codes for more information.\n";

    // clang-format off
    static constexpr BidirectionalMap abortTypes =
    {
        ABORT_MCU_HWC,   "mcu:HWC",
        ABORT_PTMU,      "ptm:u",
        ABORT_CFGU,      "cfg:u",
        ABORT_AC,        "ac:u",
        ABORT_FRD,       "frd:u",
        ABORT_SOC,       "soc:u",
        ABORT_Y2R,       "y2r:u",
        ABORT_HID_ACCEL, "HIDUSER_Accelerometer",
        ABORT_HID_GYRO,  "HIDUSER_Gyroscope"
    };
    // clang-format on

} // namespace love
