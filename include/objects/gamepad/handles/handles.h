#pragma once

#if defined (_3DS)
    struct HidVibrationValue {};

    struct SixAxisSensorValues
    {
        accelVector accelerometer;
        angularRate gyroscope;
    };
#endif

struct LOVE_Vibration
{
    float left;
    float right;
};

struct LOVE_VibrationHandle
{
    HidVibrationValue values[2];
    u32 handles[2];
};

struct LOVE_GyroscopeHandle
{
    SixAxisSensorValues value;
    u32 handles[2];
};

namespace love::gamepad
{
    class Handles
    {
        public:
            struct HandleCounts
            {
                size_t gyro;
                size_t vibration;
            };

            Handles(size_t id);
            ~Handles();

            bool SendVibration(const LOVE_Vibration & vibration);
            void ReadSixAxis(SixAxisSensorValues & values);

        private:
            void _SetVibrationData(HidVibrationValue * vibration, float value);

            LOVE_VibrationHandle vibration;
            LOVE_GyroscopeHandle gyro;

            HandleCounts handleCount;

            size_t id;
    };
}
