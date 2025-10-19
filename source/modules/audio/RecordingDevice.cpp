#include "modules/audio/RecordingDevice.hpp"

namespace love
{
    Type RecordingDeviceBase::type("RecordingDevice", &Object::type);

    RecordingDeviceBase::RecordingDeviceBase()
    {}

    RecordingDeviceBase::~RecordingDeviceBase()
    {}
} // namespace love
