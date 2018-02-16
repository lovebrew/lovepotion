#pragma once

class Gamepad
{
	public:
		Gamepad(int id);

		std::pair<std::string, float> ScanAxes();
		std::string ScanButtons(bool down);

		bool IsDown(const std::string & button);
		int GetID();

	private:
		HidControllerID GetInternalID();
		
		int id;
		JoystickPosition sticks[2];
};

extern std::vector<Gamepad *> controllers;