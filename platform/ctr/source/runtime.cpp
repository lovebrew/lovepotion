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

    static std::unique_ptr<uint32_t, love::FreeDeleter> socBuffer;
    static constexpr int SOC_BUFFER_SIZE  = 0x100000;
    static constexpr int SOC_BUFFER_ALIGN = 0x1000;

    void userAppInit()
    {
        osSetSpeedupEnable(true);

        tryInit(std::bind_front(romfsInit), love::ABORT_ROMFS);

        /* raw battery info */
        tryInit(std::bind_front(mcuHwcInit), love::ABORT_MCU_HWC);

        /* charging state */
        tryInit(std::bind_front(ptmuInit), love::ABORT_PTMU);

        /* region information and fonts */
        tryInit(std::bind_front(cfguInit), love::ABORT_CFGU);

        /* network state */
        tryInit(std::bind_front(acInit), love::ABORT_AC);

        /* friend code */
        tryInit(std::bind_front(frdInit), love::ABORT_FRD);

        /* wireless */
        socBuffer.reset((uint32_t*)aligned_alloc(SOC_BUFFER_ALIGN, SOC_BUFFER_SIZE));
        tryInit(std::bind_front(socInit, socBuffer.get(), SOC_BUFFER_SIZE), love::ABORT_SOC);

        /* theora video conversion */
        tryInit(std::bind_front(y2rInit), love::ABORT_Y2R);
    }

    void userAppExit()
    {
        y2rExit();

        socExit();

        frdExit();

        acExit();

        cfguExit();

        ptmuExit();

        romfsExit();

        mcuHwcExit();
    }
}
