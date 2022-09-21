#include <utilities/bidirectionalmap.hpp>
#include <utilities/guid.hpp>

// clang-format off
/*
** gamepad information structure list
** notes:
** nothing has a hat, so that's zero (except unknown)
** JOYCON_PAIR have an extra handle for gyro/accelerometer
** WII_U_GAMEPAD includes the HOME button as valid input
** WII_REMOTE has an accelerometer sensor, but no gyroscope
** WII_REMOTE_NUNCHUCK includes two accelerometer sensors
----
** device guid values are generated from the following site:
** https://www.guidgenerator.com/online-guid-generator.aspx
*/
static constexpr love::guid::GamepadInfo gamepadInfo[] =
{   /* buttonCount, axisCount, hatCount, name, guid */
    { -1,  -1, -1, "Unknown",                           "{}"                                     }, // GAMEPAD_TYPE_UNKNOWN
    { 12,   8,  0, "Nintendo 3DS",                      "{B58A259A-13AA-46E0-BDCB-31898EDAB24E}" }, // GAMEPAD_TYPE_NINTENDO_3DS
    { 12,  12,  0, "New Nintendo 3DS",                  "{7BC9702D-7D81-4EBB-AD4F-8C94076588D5}" }, // GAMEPAD_TYPE_NEW_NINTENDO_3DS
    { 14,   6,  0, "Nintendo Switch",                   "{6EBE242C-820F-46E1-9A66-DC8200686D51}" }, // GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD
    { 14,   6,  0, "Nintendo Switch Pro Controller",    "{42ECF5C5-AFA5-4EDE-B1A2-4E9C2287559A}" }, // GAMEPAD_TYPE_NINTENDO_SWITCH_PRO
    {  7,   6,  0, "Joy-Con L",                         "{660EBC7E-3953-4B74-8406-AD5992FCC5C7}" }, // GAMEPAD_TYPE_JOYCON_LEFT
    {  7,   6,  0, "Joy-Con R",                         "{AD770831-A7E4-41A8-8DD0-FD48323E0043}" }, // GAMEPAD_TYPE_JOYCON_RIGHT
    { 14,  18,  0, "Joy-Con Pair",                      "{701B198B-9AD9-4730-8EEB-EBECF707B9DF}" }, // GAMEPAD_TYPE_JOYCON_PAIR
    { 15,   6,  0, "Nintendo Wii U Gamepad",            "{62998927-C43D-41F5-B6B1-D22CBF031D91}" }, // GAMEPAD_TYPE_WII_U_GAMEPAD
    {  8,   3,  0, "Nintendo Wii Remote",               "{02DC4D7B-2480-4678-BB06-D9AEDC3DE29B}" }, // GAMEPAD_TYPE_WII_REMOTE
    {  8,   6,  0, "Nintendo Wii Remote with Nunchuck", "{C0E2DDE5-25DF-4F7D-AEA6-4F25DE2FC385}" }, // GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK
    { 13,   6,  0, "Nintendo Wii Classic Controller",   "{B4F6A311-8228-477D-857B-B875D891C46D}" }, // GAMEPAD_TYPE_WII_CLASSIC
    { 15,   6,  0, "Nintendo Wii Pro Controller",       "{36895D3B-A724-4F46-994C-64BCE736EBCB}" }  // GAMEPAD_TYPE_WII_PRO
};
static_assert(sizeof(gamepadInfo) / sizeof(love::guid::GamepadInfo) == love::guid::GAMEPAD_TYPE_MAX_ENUM,
              "Update the gamepadInfo array when adding or removing a GamepadInfo");
// clang-format on

int love::guid::GetGamepadButtonCount(GamepadType type)
{
    return gamepadInfo[type].buttonCount;
}

int love::guid::GetGamepadAxisCount(GamepadType type)
{
    return gamepadInfo[type].axisCount;
}

int love::guid::GetGamepadHatCount(GamepadType type)
{
    return gamepadInfo[type].hatCount;
}

const char* love::guid::GetGamepadName(GamepadType type)
{
    return gamepadInfo[type].name;
}

const char* love::guid::GetGamepadGUID(GamepadType type)
{
    return gamepadInfo[type].guid;
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
    "newnintendo3ds",    love::guid::GAMEPAD_TYPE_NEW_NINTENDO_3DS,
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
