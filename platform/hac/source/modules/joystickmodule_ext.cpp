#include <modules/joystickmodule_ext.hpp>

#include <algorithm>
#include <memory>

using namespace love;

JoystickModule<Console::HAC>::JoystickModule()
{
    for (int index = 0; index < this->GetCurrentJoystickCount(); index++)
        this->AddJoystick(index);
}

int JoystickModule<Console::HAC>::GetCurrentJoystickCount(HidsysUniquePadId* ids)
{
    HidsysUniquePadId _ids[MAX_JOYSTICKS] {};
    int32_t total;

    hidsysGetUniquePadIds(_ids, MAX_JOYSTICKS, &total);

    if (ids != nullptr)
        std::copy_n(_ids, total, ids);

    return total;
}

::Joystick* JoystickModule<Console::HAC>::AddJoystick(int index)
{}
