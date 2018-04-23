#pragma once

class Gamepad
{
	public:
		Gamepad(int id, SDL_Joystick * handle);

		float GetAxis(int axis);
		int GetButtonCount();
		std::string GetName();

		void SetVibration(double left, double right);
		void SetVibration();

		bool IsVibrationSupported();

		bool IsConnected();

		bool IsDown(const std::string & button);
		int GetID();

		void ClampAxis(float & x); 
	private:
		HidControllerID GetInternalID();

		int id;
		SDL_Joystick * joystickHandle;
		u32 vibrationHandles[2];
		HidVibrationValue vibration[2];
};

extern std::vector<Gamepad *> controllers;