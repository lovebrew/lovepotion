#pragma once

class Gamepad
{
	public:
		Gamepad(int id);

		void ScanAxes(std::pair<std::string, float> & data, HidControllerJoystick joystick);
		std::string ScanButtons(bool down);

		bool IsDown(const std::string & button);
		int GetID();

	private:
		HidControllerID GetInternalID();
		void ClampAxis(float & x); 
		
		int id;
		JoystickPosition sticks[2];
};

extern std::vector<Gamepad *> controllers;