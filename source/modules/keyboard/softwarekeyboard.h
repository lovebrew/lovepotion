#pragma once

namespace love
{
	class SoftwareKeyboard
	{
		public:
			SoftwareKeyboard();
			char * GetText();

		private:
			char text[255];
			
			SwkbdState keyboard;

			SwkbdStatusData status;
			SwkbdLearningData learning;
			SwkbdButton button = SWKBD_BUTTON_RIGHT;
	};
}

static SwkbdCallbackResult Close(void * user, const char ** ppMessage, const char * text, size_t textlen);