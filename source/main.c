#include <3ds.h>
#include <stdio.h>

int main(int argc, char **argv) {

	gfxInitDefault();

	consoleInit(GFX_BOTTOM, NULL);

	printf("Hello, World!");

	while (aptMainLoop()) {

		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break;

		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();

	}


}