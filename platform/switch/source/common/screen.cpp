#include "common/screen.h"

#include "common/bidirectionalmap.h"
#include "deko3d/deko.h"
#include "modules/graphics/graphics.h"

using namespace love;

int Screen::GetWidth(RenderScreen)
{
    if (::deko3d::Instance().IsHandheldMode())
        return Screen::HANDHELD_WIDTH;

    return Screen::DOCKED_WIDTH;
}

int Screen::GetHeight()
{
    if (::deko3d::Instance().IsHandheldMode())
        return Screen::HANDHELD_HEIGHT;

    return Screen::DOCKED_HEIGHT;
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
