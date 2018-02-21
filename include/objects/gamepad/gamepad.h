#pragma once

class Gamepad
{
	public:
		Gamepad(int id);

		void ScanAxes(std::pair<std::string, float> & data, HidControllerJoystick joystick, int axis);
		std::string ScanButtons(bool down);
		
		bool GetAxis(float & value, int axis);
		int GetButtonCount();
		std::string GetName();

		void SetVibration(float value);

		bool IsDown(const std::string & button);
		int GetID();

	private:
		HidControllerID GetInternalID();
		void ClampAxis(float & x); 
		float ReadAxis(HidControllerJoystick joystick, bool horizontal);

		int id;
		JoystickPosition sticks[2];
};

extern std::vector<Gamepad *> controllers;