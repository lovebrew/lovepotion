#pragma once

class Gamepad
{
	public:
		Gamepad(int id);

		float GetAxis(int axis);
		bool IsDown(std::string button);
		std::string ScanInput();

	private:
		int id;
		JoystickPosition sticks[2];
};

extern std::vector<Gamepad *> controllers;