#include "common/screen.h"

#include "citro2d/citro.h"
#include "common/bidirectionalmap.h"
#include "modules/graphics/graphics.h"

using namespace love;

int Screen::GetWidth(RenderScreen screen)
{
    if (::citro2d::Instance().Get3D())
    {
        switch (screen)
        {
            case (int)Screen::CtrScreen::CTR_SCREEN_LEFT:
            case (int)Screen::CtrScreen::CTR_SCREEN_RIGHT:
            default:
                return Screen::TOP_WIDTH;
            case (int)Screen::CtrScreen::CTR_SCREEN_BOTTOM:
                return Screen::BOTTOM_WIDTH;
        }
    }
    else
    {
        switch (screen)
        {
            case (int)Screen::Ctr2dScreen::CTR_2D_SCREEN_TOP:
            default:
                if (::citro2d::Instance().GetWide())
                    return Screen::TOP_WIDE_WIDTH;
                return Screen::TOP_WIDTH;
            case (int)Screen::Ctr2dScreen::CTR_2D_SCREEN_BOTTOM:
                return Screen::BOTTOM_WIDTH;
        }
    }
}

int Screen::GetHeight()
{
    return Screen::HEIGHT;
}

// clang-format off
constexpr auto ScreenTypes = BidirectionalMap<>::Create(
    "left",   Screen::CtrScreen::CTR_SCREEN_LEFT,
    "right",  Screen::CtrScreen::CTR_SCREEN_RIGHT,
    "bottom", Screen::CtrScreen::CTR_SCREEN_BOTTOM
);

constexpr auto ScreenTypes2d = BidirectionalMap<>::Create(
    "top",    Screen::Ctr2dScreen::CTR_2D_SCREEN_TOP,
    "bottom", Screen::Ctr2dScreen::CTR_2D_SCREEN_BOTTOM
);
// clang-format on

bool Screen::GetConstant(const char* in, RenderScreen& out)
{
    if (::citro2d::Instance().Get3D())
        return Screen::FindSetCast<CtrScreen>(ScreenTypes, in, out);

    return Screen::FindSetCast<Ctr2dScreen>(ScreenTypes2d, in, out);
}

bool Screen::GetConstant(RenderScreen in, const char*& out)
{
    if (::citro2d::Instance().Get3D())
        return Screen::ReverseFindSetCast<CtrScreen>(ScreenTypes, in, out);

    return Screen::ReverseFindSetCast<Ctr2dScreen>(ScreenTypes2d, in, out);
}

std::vector<const char*> Screen::GetConstants(RenderScreen)
{
    if (::citro2d::Instance().Get3D())
        return ScreenTypes.GetNames();

    return ScreenTypes2d.GetNames();
}
