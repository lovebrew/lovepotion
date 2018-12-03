#pragma once

class Gamepad : public Object
{
    public:
        Gamepad(int id);

        float GetAxis(uint axis);
        float GetGamepadAxis(const std::string & name);

        int GetButtonCount();
        std::string GetName();

        void Update(float delta);
        void SetLayout(const std::string & mode, const std::string & holdType);

        void SetVibration(double left, double right, double duration);

        bool IsVibrationSupported();

        bool IsConnected();

        bool IsGamepadDown(const std::string & button);
        bool IsDown(uint button);
        
        int GetID();

        void ClampAxis(float & x); 
    private:
        HidControllerID GetInternalID();
        void SetVibrationData(HidVibrationValue & value, double amplitude);
        

        int id;
        std::string layout;

        std::array<HidControllerID, 2> joycon;
        std::pair<SDL_Joystick *, SDL_Joystick *> joysticks;

        double vibrationDuration;
        u32 vibrationHandles[2][2];
        HidVibrationValue vibration[2];
};

extern std::vector<Gamepad *> controllers;