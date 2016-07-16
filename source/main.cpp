#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>

#include "utils.h"
#include "menu.h"
#include "ui.h"

PrintConsole top;
PrintConsole bot;

void audioPlay(const char* fname, int chn, u32 flags, u32 sampleRate) {
	FILE* file = fopen(fname, "rb");
	fseek(file, 0, SEEK_END);
	off_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	u8* buffer = (u8*)linearAlloc(size);
	fread(buffer, 1, size, file);
	fclose(file);
	csndPlaySound(chn, flags, sampleRate, 1, 0, buffer, buffer, size);
	linearFree(buffer);
}

void audioStop(int chn) {
	csndExecCmds(true);
	CSND_SetPlayState(chn, 0);
    csndExecCmds(true);
	CSND_SetPlayState(chn, 0);
}

int main(int argc, char** argv)
{
    newsInit();
    fsInit();
    amInit();
    cfguInit();
    romfsInit();
    csndInit();
    gfxInitDefault();
    consoleInit(GFX_TOP, &top);
    consoleInit(GFX_BOTTOM, &bot);
    consoleSelect(&top);

    // create NotifyMii folder
    char folderPath[] = "/NotifyMii";
    mkdir(folderPath, 0777);

    // general stuff
    u8 menu = 0;
    char title[32];
    char message[0x1780];
    u8* image = (u8*)malloc(0xC800);
    u32 imgSize = 0;
    u64 processID = 0;

    // play music
    audioPlay("romfs:/audio/bgm.pcm", 31, SOUND_FORMAT_16BIT | SOUND_REPEAT, 32000); // using channel 31 to prevent swkbd to stop the music

    while (aptMainLoop())
    {
        consoleSelect(&bot);
        consoleClear();
        printInfo(MODE_NONE, true);
        switch (menu) {
            case MENU_MAIN: { menuMain(&menu); break; }
            case MENU_NEWS_ADD_TITLE: { menuNewsAddTitle(&menu, title); break; }
            case MENU_NEWS_LIST: { menuNewsList(&menu); break; }
            case MENU_UPDATE: { menuUpdate(&menu); break; }
            case MENU_NEWS_ADD_MESSAGE: { menuNewsAddMessage(&menu, message); break; }
            case MENU_NEWS_ADD_IMAGE: { menuNewsAddImage(&menu, image, &imgSize); break; }
            case MENU_NEWS_ADD_PROCESS: { menuNewsAddProcess(&menu, &processID); break; }
            case MENU_NEWS_ADD: { menuAddNews(&menu, title, message, image, imgSize, processID); break; }
            default: { menu = MENU_EXIT; break; }
        }
        if (menu==MENU_EXIT) break;
    }

    free(image);
    audioStop(31);
    consoleSelect(&top);
    consoleClear();
    consoleSelect(&bot);
    consoleClear();
    gfxExit();
    csndExit();
    romfsExit();
    cfguExit();
    amExit();
    fsExit();
    newsExit();
    return 0;
}
