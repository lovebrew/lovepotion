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

		void SetVibration(float amp_low, float freq_low, float amp_high, float freq_high);
		bool IsVibrationSupported();

		bool IsDown(const std::string & button);
		int GetID();

		void ClampAxis(float & x); 
	private:
		HidControllerID GetInternalID();
		float ReadAxis(HidControllerJoystick joystick, bool horizontal);

		int id;
		JoystickPosition sticks[2];
		
		/*u32 vibrationDeviceHandles[2][2];
		HidVibrationValue vibrationValue;
		HidVibrationValue vibrationValue_stop;
		HidVibrationValue vibrationValues[2];*/
};

extern std::vector<Gamepad *> controllers;