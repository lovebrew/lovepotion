#include "modules/system/System.hpp"
#include "common/Result.hpp"

#include <coreinit/memory.h>

#include <cstring>

namespace love
{
    System::System() : settings {}
    {
        this->mcpHandle = MCP_Open();
        this->ucHandle  = UCOpen();

        nn::act::Initialize();

        this->account = nn::act::GetDefaultAccount();
    }

    System::~System()
    {
        if (this->mcpHandle != 0)
            MCP_Close(this->mcpHandle);

        if (this->ucHandle != 0)
            UCClose(this->ucHandle);

        nn::act::Finalize();
    }

    int System::getProcessorCount() const
    {
        return OSGetCoreCount();
    }

    System::PowerState System::getPowerInfo(int& seconds, int& percent) const
    {
        percent = -1;
        seconds = -1;

        return PowerState::POWER_NO_BATTERY;
    }

    std::vector<std::string> System::getPreferredLocales() const
    {
        std::vector<std::string> locales {};

        UCSysConfig config {};
        uint32_t data = 0xFFFFFFFF;

        config.dataType = UC_DATATYPE_UNSIGNED_INT;
        config.dataSize = sizeof(data);
        config.data     = &data;

        std::strncpy(config.name, "cafe.language", sizeof(config.name));

        if (!ResultCode(UCReadSysConfig(this->ucHandle, 1, &config)))
            return locales;

        int32_t language = (*(uint32_t*)config.data);

        std::string_view result = "Unknown";
        System::getConstant(language, result);

        locales.push_back(std::string(result));

        return locales;
    }

    System::NetworkState System::getNetworkInfo(int32_t& signal) const
    {
        BOOL status = 0;
        signal      = -1;

        if (!ResultCode(ACIsApplicationConnected(&status)))
            return NetworkState::NETWORK_UNKNOWN;

        if (!status)
            return NetworkState::NETWORK_DISCONNECTED;

        signal = 100;
        return NetworkState::NETWORK_CONNECTED;
    }

    bool System::getFriendInfo(FriendInfo& info) const
    {
        char username[nn::act::AccountIdSize] { 0 };

        if (!nn::act::IsNetworkAccount())
            return false;

        if (!ResultCode(nn::act::GetAccountId(username)))
            return false;

        info.username = username;

        return true;
    }

    int System::getMemorySize() const
    {
        return 0;
    }

    bool System::getInfo(ProductInfo& info) const
    {
        return true;
    }
} // namespace love
