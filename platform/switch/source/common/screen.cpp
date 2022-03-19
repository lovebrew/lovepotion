#include "common/screen.h"

#include "common/bidirectionalmap.h"

using namespace love;

int Screen::GetWidth(RenderScreen)
{
    return Screen::WIDTH;
}

int Screen::GetHeight(RenderScreen)
{
    return Screen::HEIGHT;
}

// clang-format off
constexpr auto ScreenTypes = BidirectionalMap<>::Create(
    "default", Screen::HacScreen::HAC_SCREEN_DEFAULT
);
// clang-format on

bool Screen::GetConstant(const char* in, RenderScreen& out)
{
    return Screen::FindSetCast<HacScreen>(ScreenTypes, in, out);
}

bool Screen::GetConstant(RenderScreen in, const char*& out)
{
    return Screen::ReverseFindSetCast<HacScreen>(ScreenTypes, in, out);
}

std::vector<const char*> Screen::GetConstants(RenderScreen)
{
    return ScreenTypes.GetNames();
}
