#include <utilities/swkbdthread.hpp>

#include <nn/swkbd.h>
#include <vpad/input.h>
#include <whb/gfx.h>

using namespace love;

SwkbdThread::~SwkbdThread()
{}

void SwkbdThread::SetFinish()
{
    this->finish = true;
}

void SwkbdThread::ThreadFunction()
{
    while (!this->finish)
    {
        VPADStatus vpadStatus {};
        VPADRead(VPAD_CHAN_0, &vpadStatus, 1, nullptr);
        VPADGetTPCalibratedPoint(VPAD_CHAN_0, &vpadStatus.tpNormal, &vpadStatus.tpNormal);

        nn::swkbd::ControllerInfo controllerInfo {};
        controllerInfo.vpad = &vpadStatus;

        nn::swkbd::Calc(controllerInfo);

        if (nn::swkbd::IsNeedCalcSubThreadFont())
            nn::swkbd::CalcSubThreadFont();

        if (nn::swkbd::IsNeedCalcSubThreadPredict())
            nn::swkbd::CalcSubThreadPredict();

        if (nn::swkbd::IsDecideOkButton(nullptr))
        {
            nn::swkbd::DisappearInputForm();
            this->finish = true;
        }

        WHBGfxBeginRender();

        WHBGfxBeginRenderTV();
        WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        nn::swkbd::DrawTV();
        WHBGfxFinishRenderTV();

        WHBGfxBeginRenderDRC();
        WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        nn::swkbd::DrawDRC();
        WHBGfxFinishRenderDRC();

        WHBGfxFinishRender();
    }
}
