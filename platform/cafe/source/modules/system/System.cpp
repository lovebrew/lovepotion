#include "modules/system/System.hpp"
#include "common/Result.hpp"

#include <cstring>

namespace love
{
    System::System() : SystemBase()
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
        percent = 100;
        return PowerState::POWER_UNKNOWN;
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

        if (!Result(UCReadSysConfig(this->ucHandle, 1, &config)))
            return locales;

        int32_t language = (*(uint32_t*)config.data);

        std::string_view result = "Unknown";
        System::getConstant(language, result);

        locales.push_back(std::string(result));

        return locales;
    }

    System::NetworkState System::getNetworkInfo(uint8_t& signal) const
    {
        int32_t status = 0;
        auto state     = NetworkState::NETWORK_UNKNOWN;

        if (!Result(ACIsApplicationConnected(&status)))
            return state;

        signal = (status > 0) ? 100 : 0;
        state = (status > 0) ? NetworkState::NETWORK_CONNECTED : NetworkState::NETWORK_DISCONNECTED;
    }

    System::FriendInfo System::getFriendInfo() const
    {
        char username[nn::act::AccountIdSize] { 0 };

        if (!nn::act::IsNetworkAccount())
            return FriendInfo();

        if (!Result(nn::act::GetAccountId(username)))
            return FriendInfo();

        return FriendInfo(username);
    }

    static std::string getVersion(int32_t handle)
    {
        MCPSystemVersion version {};
        if (!Result(MCP_GetSystemVersion(handle, &version)))
            return "Unknown";

        return std::format("{}.{}.{}-{}", version.major, version.minor, version.patch,
                           version.region);
    }

    static std::string getModel()
    {
        BSPHardwareVersion version = BSP_HARDWARE_VERSION_UNKNOWN;

        if (!Result(bspGetHardwareVersion(&version)))
            return "Unknown";

        std::string_view result {};
        System::getConstant(version, result);

        return std::string(result);
    }

    static std::string getRegion(int32_t handle)
    {
        MCPSysProdSettings settings {};

        if (!Result(MCP_GetSysProdSettings(handle, &settings)))
            return "Unknown";

        std::string_view result {};
        System::getConstant(settings.product_area, result);

        return std::string(result);
    }

    System::ProductInfo System::getProductInfo() const
    {
        std::string model   = getModel();
        std::string version = getVersion(this->mcpHandle);
        std::string region  = getRegion(this->mcpHandle);

        return { model, version, region };
    }
} // namespace love
