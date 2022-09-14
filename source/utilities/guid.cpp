#include <utilities/bidirectionalmap.hpp>
#include <utilities/guid.hpp>

std::string love::guid::GetDeviceGUID(GamepadType type)
{
    switch (type)
    {
        case GAMEPAD_TYPE_NINTENDO_3DS:
            return "{B58A259A-13AA-46E0-BDCB-31898EDAB24E}";
        case GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
            return "{42ECF5C5-AFA5-4EDE-B1A2-4E9C2287559A}";
        case GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD:
            return "{6EBE242C-820F-46E1-9A66-DC8200686D51}";
        case GAMEPAD_TYPE_JOYCON_LEFT:
            return "{660EBC7E-3953-4B74-8406-AD5992FCC5C7}";
        case GAMEPAD_TYPE_JOYCON_RIGHT:
            return "{AD770831-A7E4-41A8-8DD0-FD48323E0043}";
        case GAMEPAD_TYPE_JOYCON_PAIR:
            return "{701B198B-9AD9-4730-8EEB-EBECF707B9DF}";
        case GAMEPAD_TYPE_WII_U_GAMEPAD:
            return "{62998927-C43D-41F5-B6B1-D22CBF031D91}";
        case GAMEPAD_TYPE_WII_REMOTE:
            return "{02DC4D7B-2480-4678-BB06-D9AEDC3DE29B}";
        case GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            return "{C0E2DDE5-25DF-4F7D-AEA6-4F25DE2FC385}";
        case GAMEPAD_TYPE_WII_CLASSIC:
            return "{B4F6A311-8228-477D-857B-B875D891C46D}";
        case GAMEPAD_TYPE_WII_PRO:
            return "{36895D3B-A724-4F46-994C-64BCE736EBCB}";
        default:
            break;
    }

    return std::string {};
}

std::string love::guid::GetDeviceName(GamepadType type)
{
    switch (type)
    {
        case GAMEPAD_TYPE_NINTENDO_3DS:
            return "Nintendo 3DS";
        case GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
            return "Nintendo Switch Pro Controller";
        case GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD:
            return "Nintendo Switch";
        case GAMEPAD_TYPE_JOYCON_LEFT:
            return "Joy-Con L";
        case GAMEPAD_TYPE_JOYCON_RIGHT:
            return "Joy-Con R";
        case GAMEPAD_TYPE_JOYCON_PAIR:
            return "Joy-Con Pair";
        case GAMEPAD_TYPE_WII_U_GAMEPAD:
            return "Nintendo Wii U Gamepad";
        case GAMEPAD_TYPE_WII_REMOTE:
            return "Nintendo Wii Remote";
        case GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            return "Nintendo Wii Remote and Nunchuck";
        case GAMEPAD_TYPE_WII_CLASSIC:
            return "Nintendo Wii Classic Controller";
        case GAMEPAD_TYPE_WII_PRO:
            return "Nintendo Wii Pro Controller";
        default:
            break;
    }

    return std::string {};
}

bool love::guid::GetDeviceInfo(GamepadType type, DeviceInfo& info)
{
    info.vendorId       = 0x057E;
    info.productId      = 0x0000;
    info.productVersion = 0x0001;

    switch (type)
    {
        case GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD:
            info.productId = 0x2000;
            break;
        case GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
            info.productId = 0x2009;
            break;
        case GAMEPAD_TYPE_JOYCON_LEFT:
            info.productId = 0x2006;
        case GAMEPAD_TYPE_JOYCON_RIGHT:
            info.productId = 0x2007;
        case GAMEPAD_TYPE_WII_REMOTE:
            info.productId = 0x0306;
            break;
        default:
            return false;
    }

    return true;
}

// clang-format off
constexpr auto gamepadTypes = BidirectionalMap<>::Create(
    "unknown",           love::guid::GAMEPAD_TYPE_UNKNOWN,
    "nintendo3ds",       love::guid::GAMEPAD_TYPE_NINTENDO_3DS,
    "switchpro",         love::guid::GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,
    "switchhandheld",    love::guid::GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD,
    "joyconleft",        love::guid::GAMEPAD_TYPE_JOYCON_LEFT,
    "joyconright",       love::guid::GAMEPAD_TYPE_JOYCON_RIGHT,
    "joyconpair",        love::guid::GAMEPAD_TYPE_JOYCON_PAIR,
    "wiiugamepad",       love::guid::GAMEPAD_TYPE_WII_U_GAMEPAD,
    "wiiremote",         love::guid::GAMEPAD_TYPE_WII_REMOTE,
    "wiiremotenunchuck", love::guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK,
    "wiiclassic",        love::guid::GAMEPAD_TYPE_WII_CLASSIC,
    "wiipro",            love::guid::GAMEPAD_TYPE_WII_PRO
);
// clang-format on

bool love::guid::GetConstant(const char* in, GamepadType& out)
{
    return gamepadTypes.Find(in, out);
}

bool love::guid::GetConstant(GamepadType in, const char*& out)
{
    return gamepadTypes.ReverseFind(in, out);
}

std::vector<const char*> love::guid::GetConstants(GamepadType)
{
    return gamepadTypes.GetNames();
}
