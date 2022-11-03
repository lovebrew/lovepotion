#include <switch.h>

#include <utilities/result.hpp>

#include <cstring>

extern "C"
{
    static void tryInit(love::ResultCode result, love::AbortCode code)
    {
        if (result.Success() || love::g_EarlyExit)
            return;

        ErrorApplicationConfig config {};

        std::optional<const char*> header;
        if ((header = love::abortTypes.Find(code)) && code != love::ABORT_APPLET)
        {
            char message[0x800] {};
            int32_t info = R_DESCRIPTION(result);

            snprintf(message, sizeof(message), love::ABORT_FORMAT, *header, (int32_t)result, info);

            errorApplicationCreate(&config, message, nullptr);
            errorApplicationSetNumber(&config, result);
            errorApplicationShow(&config);
        }

        love::g_EarlyExit = true;
    }

    void userAppInit()
    {
        {
            const auto appletType = appletGetAppletType();
            const bool isValid    = (appletType == AppletType_Application ||
                                  appletType == AppletType_SystemApplication);

            tryInit(isValid ? 0 : -1, love::ABORT_APPLET);

            ErrorSystemConfig config {};
            errorSystemCreate(&config, love::TITLE_TAKEOVER_ERROR, nullptr);
            errorSystemShow(&config);

            if (love::g_EarlyExit)
                return;
        }

        /* system fonts */
        tryInit(plInitialize(PlServiceType_User), love::ABORT_PLU);

        /* wireless */
        tryInit(nifmInitialize(NifmServiceType_User), love::ABORT_NIFM);

        /* accounts */
        tryInit(accountInitialize(AccountServiceType_Application), love::ABORT_ACC);

        /* settings */
        tryInit(setInitialize(), love::ABORT_SET);

        /* system settings */
        tryInit(setsysInitialize(), love::ABORT_SETSYS);

        /* battery charge and state */
        tryInit(psmInitialize(), love::ABORT_PSM);

        /* wireless */
        tryInit(socketInitializeDefault(), love::ABORT_SOCKETS);

        /* friends */
        tryInit(friendsInitialize(FriendsServiceType_Viewer), love::ABORT_FRIENDV);

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
