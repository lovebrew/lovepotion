#include <switch.h>

#include <utilities/result.hpp>

#include <cstring>
#include <functional>

extern "C"
{
    static void tryInit(std::function<love::ResultCode()> serviceInit, love::AbortCode code)
    {
        if (!serviceInit || love::g_EarlyExit)
            return;

        love::ResultCode result;
        if ((result = serviceInit()); result.Success())
            return;

        std::optional<const char*> header;
        if ((header = love::abortTypes.Find(code)) && code != love::ABORT_APPLET)
        {
            static char message[0x100] {};
            int32_t info = R_DESCRIPTION(result);

            snprintf(message, sizeof(message), love::ABORT_FORMAT, *header, (int32_t)result, info);

            ErrorApplicationConfig config {};
            errorApplicationCreate(&config, message, nullptr);
            errorApplicationSetNumber(&config, result);
            errorApplicationShow(&config);
        }
        else
        {
            ErrorSystemConfig config {};
            errorSystemCreate(&config, love::TITLE_TAKEOVER_ERROR, nullptr);
            errorSystemShow(&config);
        }

        love::g_EarlyExit = true;
    }

    void userAppInit()
    {
        {
            const auto appletType = appletGetAppletType();
            const bool isValid    = (appletType == AppletType_Application ||
                                  appletType == AppletType_SystemApplication);

            tryInit(std::bind_front([&]() { return (isValid) ? 0 : -1; }), love::ABORT_APPLET);
        }

        /* system fonts */
        tryInit(std::bind_front(plInitialize, PlServiceType_User), love::ABORT_PLU);

        /* wireless */
        tryInit(std::bind_front(nifmInitialize, NifmServiceType_User), love::ABORT_NIFM);

        /* accounts */
        tryInit(std::bind_front(accountInitialize, AccountServiceType_Application),
                love::ABORT_ACC);

        /* settings */
        tryInit(std::bind_front(setInitialize), love::ABORT_SET);

        /* system settings */
        tryInit(std::bind_front(setsysInitialize), love::ABORT_SETSYS);

        /* battery charge and state */
        tryInit(std::bind_front(psmInitialize), love::ABORT_PSM);

        /* wireless */
        tryInit(std::bind_front(socketInitializeDefault), love::ABORT_SOCKETS);

        /* friends */
        tryInit(std::bind_front(friendsInitialize, FriendsServiceType_Viewer), love::ABORT_FRIENDV);

        /* initialize controllers -- 4 players max */
        padConfigureInput(0x04, HidNpadStyleSet_NpadStandard);

        /* initialize touch screen */
        hidInitializeTouchScreen();
    }

    void userAppExit()
    {
        friendsExit();

        socketExit();

        psmExit();

        setsysExit();

        setExit();

        accountExit();

        nifmExit();

        plExit();
    }
}
