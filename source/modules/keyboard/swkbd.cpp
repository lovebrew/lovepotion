#include "common/runtime.h"
#include "swkbd.h"

bool CLOSE_KEYBOARD = false;

using love::SwKbd;

SwKbd::SwKbd()
{
	swkbdInit(&this->keyboard, SWKBD_TYPE_NORMAL, 2, -1);

	swkbdSetInitialText(&this->keyboard, this->text);
	swkbdSetHintText(&this->keyboard, "Enter text");

	swkbdSetFeatures(&this->keyboard, SWKBD_DEFAULT_QWERTY);
	swkbdSetValidation(&this->keyboard, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);

	swkbdSetFilterCallback(&this->keyboard, Close, NULL);

	memset(this->text, 0, sizeof(this->text));

	this->button = swkbdInputText(&this->keyboard, this->text, sizeof(this->text));
}

SwkbdCallbackResult Close(void * user, const char ** ppMessage, const char * text, size_t textlen)
{
	CLOSE_KEYBOARD = true;

	return SWKBD_CALLBACK_OK;
}

char * SwKbd::GetText()
{
	return this->text;
}