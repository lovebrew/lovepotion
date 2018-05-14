#pragma once

class Gamepad
{
	public:
		Gamepad(int id);

		float GetAxis(int axis);
		int GetButtonCount();
		std::string GetName();

		void Update(float delta);

		void SetVibration(double left, double right, double duration);

		bool IsVibrationSupported();

		bool IsConnected();

		bool IsGamepadDown(const std::string & button);
		bool IsDown(int button);
		
		int GetID();

		void ClampAxis(float & x); 
	private:
		HidControllerID GetInternalID();
		void SetVibrationData(HidVibrationValue & value, double amplitude);
		
		double vibrationDuration;

		int id;
		u32 vibrationHandles[2][2];
		HidVibrationValue vibration[2];
};

extern std::vector<Gamepad *> controllers;