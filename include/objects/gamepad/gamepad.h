#pragma once

class Gamepad
{
	public:
		Gamepad(int id);

		void ScanAxes(std::pair<std::string, float> & data);
		std::string ScanButtons(bool down);

		bool IsDown(const std::string & button);
		int GetID();

	private:
		HidControllerID GetInternalID();
		
		int id;
		JoystickPosition stick;
};

extern std::vector<Gamepad *> controllers;