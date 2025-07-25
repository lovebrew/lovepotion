#include <3ds.h>

#include <utilities/result.hpp>

#include <cstring>
#include <functional>
#include <memory>

extern "C"
{
    static void tryInit(std::function<love::ResultCode()> initFunction, love::AbortCode code)
    {
        if (!initFunction || love::g_EarlyExit)
            return;

        love::ResultCode result;
        if ((result = initFunction()); result.Success())
            return;

        errorConf conf {};

        errorInit(&conf, ERROR_TEXT_WORD_WRAP, CFG_LANGUAGE_EN);
        errorCode(&conf, result);

        static char message[0x100] {};

        std::optional<const char*> header;
        if ((header = love::abortTypes.Find(code)))
            snprintf(message, sizeof(message), love::ABORT_FORMAT_KNOWN, *header, (int32_t)result,
                     R_LEVEL(result), R_SUMMARY(result), R_DESCRIPTION(result));

        errorText(&conf, message);
        errorDisp(&conf);

        love::g_EarlyExit = true;
    }

    void userAppInit()
    {
        osSetSpeedupEnable(true);

        tryInit(std::bind_front(romfsInit), love::ABORT_ROMFS);

#if !defined(__EMULATION__)
        /* raw battery info */
        tryInit(std::bind_front(mcuHwcInit), love::ABORT_MCU_HWC);
#endif

        /* charging state */
        tryInit(std::bind_front(ptmuInit), love::ABORT_PTMU);

        /* region information and fonts */
        tryInit(std::bind_front(cfguInit), love::ABORT_CFGU);

        /* network state */
        tryInit(std::bind_front(acInit), love::ABORT_AC);

        /* friend code */
        tryInit(std::bind_front(frdInit, false), love::ABORT_FRD);

        /* theora video conversion */
        tryInit(std::bind_front(y2rInit), love::ABORT_Y2R);
    }

    void userAppExit()
    {
        y2rExit();

        frdExit();

        acExit();

        cfguExit();

        ptmuExit();

        romfsExit();

#if !defined(__EMULATION__)
        mcuHwcExit();
#endif
    }
}
