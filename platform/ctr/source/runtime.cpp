#include <3ds.h>

#include <result.hpp>

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <memory>

extern "C"
{
    static void tryInit(love::ResultCode result, love::AbortCode code)
    {
        if (result.Success())
            return;

        errorConf conf {};

        errorInit(&conf, ERROR_TEXT_WORD_WRAP, CFG_LANGUAGE_EN);
        errorCode(&conf, result);

        /* max errorConf message size */
        char message[0x76C] {};

        std::optional<const char*> header;
        if ((header = love::abortTypes.Find(code)))
            snprintf(message, sizeof(message), love::ABORT_FORMAT_KNOWN, *header, (int32_t)result,
                     R_LEVEL(result), R_SUMMARY(result), R_DESCRIPTION(result));

        errorText(&conf, message);
        errorDisp(&conf);

        love::g_EarlyExit = true;
    }

    static std::unique_ptr<uint32_t, love::FreeDeleter> socBuffer;
    static constexpr int SOC_BUFFER_SIZE  = 0x100000;
    static constexpr int SOC_BUFFER_ALIGN = 0x1000;

    void userAppInit()
    {
        osSetSpeedupEnable(true);

        gfxInitDefault();

        /* raw battery info */
        tryInit(mcuHwcInit(), love::ABORT_MCU_HWC);

        /* charging state */
        tryInit(ptmuInit(), love::ABORT_PTMU);

        /* region information and fonts */
        tryInit(cfguInit(), love::ABORT_CFGU);

        /* network state */
        tryInit(acInit(), love::ABORT_AC);

        /* friend code */
        tryInit(frdInit(), love::ABORT_FRD);

        /* wireless */
        socBuffer.reset((uint32_t*)aligned_alloc(SOC_BUFFER_ALIGN, SOC_BUFFER_SIZE));
        tryInit(socInit(socBuffer.get(), SOC_BUFFER_SIZE), love::ABORT_SOC);

        /* theora video conversion */
        tryInit(y2rInit(), love::ABORT_Y2R);
    }

    void userAppExit()
    {
        y2rExit();

        socExit();

        frdExit();

        acExit();

        cfguExit();

        ptmuExit();

        mcuHwcExit();
    }
}
