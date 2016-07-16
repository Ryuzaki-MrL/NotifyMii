#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>

#include "utils.h"
#include "menu.h"
#include "ui.h"

PrintConsole top;
PrintConsole bot;

int main(int argc, char** argv)
{
    newsInit();
    fsInit();
    amInit();
    cfguInit();
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

    while (aptMainLoop())
    {
        consoleSelect(&bot);
        consoleClear();
        printInfo(MODE_NORMAL, true);
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
    consoleSelect(&top);
    consoleClear();
    consoleSelect(&bot);
    consoleClear();
    gfxExit();
    cfguExit();
    amExit();
    fsExit();
    newsExit();
    return 0;
}
