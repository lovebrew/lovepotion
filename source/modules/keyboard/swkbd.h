#pragma once

namespace love
{
	class SwKbd
	{
		public:
			SwKbd();
			char * GetText();

		private:
			char text[255];
			
			SwkbdState keyboard;

			SwkbdStatusData status;
			SwkbdLearningData learning;
			SwkbdButton button = SWKBD_BUTTON_RIGHT;
	};
}

SwkbdCallbackResult Close(void * user, const char ** ppMessage, const char * text, size_t textlen);