#pragma once

class Gamepad
{
	public:
		Gamepad(int id, SDL_Joystick * handle);

		float GetAxis(int axis);
		int GetButtonCount();
		std::string GetName();

		void SetVibration(float amp_low, float freq_low, float amp_high, float freq_high);
		bool IsVibrationSupported();

		bool IsConnected();

		bool IsDown(const std::string & button);
		int GetID();

		void ClampAxis(float & x); 
	private:
		HidControllerID GetInternalID();
		float ReadAxis(HidControllerJoystick joystick, bool horizontal);
		
		int id;
		SDL_Joystick * joystickHandle;
};

extern std::vector<Gamepad *> controllers;