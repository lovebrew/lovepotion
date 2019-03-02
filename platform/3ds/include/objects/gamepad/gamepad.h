#pragma once

class Gamepad : public Object
{
    public:
        Gamepad();

        float GetAxis(int axis);
        int GetButtonCount();
        std::string GetName();
        
        bool IsVibrationSupported();

        bool IsConnected();

        bool IsGamepadDown(const std::string & button);
        bool IsDown(int button);
        
        int GetID();

        float ClampAxis(float x);

        string GetInput(u32 key);

    private:
        bool IsButtonValid(const std::string & button);

        int id;
        bool isN3DS;
};