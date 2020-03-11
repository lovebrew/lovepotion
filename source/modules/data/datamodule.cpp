#include "common/runtime.h"
#include "modules/data/datamodule.h"

namespace love
{
    namespace data
    {
        static StringMap<ContainerType, ContainerType::CONTAINER_MAX_ENUM>::Entry containerEntries[] = {
            { "string", ContainerType::CONTAINER_STRING },
            { "data",   ContainerType::CONTAINER_DATA   }
        };

        static StringMap<ContainerType, ContainerType::CONTAINER_MAX_ENUM> containers(containerEntries, sizeof(containerEntries));
    }
}

using namespace love;

bool DataModule::GetConstant(const char * in, love::data::ContainerType & out)
{
    return love::data::containers.Find(in, out);
}

std::vector<std::string> DataModule::GetConstants()
{
    return love::data::containers.GetNames();
}
