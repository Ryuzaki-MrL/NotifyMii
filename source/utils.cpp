#include <stdio.h>
#include <3ds.h>

#include "utils.h"

void ascii2utf(u16* dst, char* src) {
	if (!src || !dst) return;
	while(*src) *(dst++)=(*src++);
	*dst=0x00;
}

void utf2ascii(char* dst, u16* src) {
	if (!src || !dst) return;
	while(*src) *(dst++)=(*(src++))&0xFF;
	*dst=0x00;
}

void waitKey() {
    while ( aptMainLoop() )
    {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown) break;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}