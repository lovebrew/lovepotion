#include "common/runtime.h"
#include "softwarekeyboard.h"

bool CLOSE_KEYBOARD = false;
using love::SoftwareKeyboard;

SoftwareKeyboard::SoftwareKeyboard()
{
	swkbdInit(&this->keyboard, SWKBD_TYPE_NORMAL, 2, -1);

	swkbdSetInitialText(&this->keyboard, this->text);
	swkbdSetHintText(&this->keyboard, "Enter text");

	swkbdSetFeatures(&this->keyboard, SWKBD_DEFAULT_QWERTY);
	swkbdSetValidation(&this->keyboard, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);

	swkbdSetFilterCallback(&this->keyboard, Close, NULL);


	this->button = swkbdInputText(&this->keyboard, this->text, sizeof(this->text));
}

static SwkbdCallbackResult Close(void * user, const char ** ppMessage, const char * text, size_t textlen)
{
	CLOSE_KEYBOARD = true;

	return SWKBD_CALLBACK_OK;
}

char * SoftwareKeyboard::GetText()
{
	return this->text;
}