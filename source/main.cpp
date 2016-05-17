#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "keyboard.h"
#include "menu.h"
#include "notification.h"
#include "utils.h"

PrintConsole top;
PrintConsole bot;

int main(int argc, char **argv)
{
    //initialization
    newsInit();
    aptInit();
    fsInit();
    amInit();
    gfxInitDefault();
    consoleInit(GFX_TOP, &top);
    consoleInit(GFX_BOTTOM, &bot);
    consoleSelect(&top);
    
    //create NotifyMii folder
    char folderPath[] = "/NotifyMii";
    mkdir(folderPath, 0777);
    
    //initialize variables
    u8 menu = 0;
    char title[32];
    char message[0x1780];
    u8* image = (u8*)malloc(0xC800);
    u32 imgSize = 0;
    bool hasImage = true;
    u64 processID = 0;
    
    //main loop
    while ( aptMainLoop() )
    {
        //clear console
        consoleSelect(&bot);
        consoleClear();
        consoleSelect(&top);
        consoleClear();
        
        //print app info
        printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.2                                    \x1b[0m");
        
        //menu loop
        switch (menu) {
            case MENU_MAIN: menuMain(&menu); break;
            case MENU_ADD_TITLE: menuAddTitle(&menu, title); break;
            case MENU_LIST: menuList(&menu); break;
            case MENU_CLEAR: menuClear(&menu); break;
            case MENU_ADD_MESSAGE_SELECT: menuAddMessageSelect(&menu); break;
            case MENU_ADD_MESSAGE_KEYBOARD: menuAddMessageKeyboard(&menu, message); break;
            case MENU_ADD_MESSAGE_FILE: menuAddMessageFile(&menu, message); break;
            case MENU_ADD_MESSAGE_NEWS: menuAddMessageNews(&menu, message); break;
            case MENU_ADD_IMAGE_SELECT: menuAddImageSelect(&menu); break;
            case MENU_ADD_IMAGE_DEFAULT: menuAddImageDefault(&menu, image, &imgSize); break;
            case MENU_ADD_IMAGE_FILE: menuAddImageFile(&menu, image, &imgSize); break;
            case MENU_ADD_IMAGE_NEWS: menuAddImageNews(&menu, image, &imgSize); break;
            case MENU_ADD_IMAGE_EMPTY: menuAddImageEmpty(&menu, image, &imgSize, &hasImage); break;
            case MENU_ADD_PROCESS_SELECT: menuAddProcessSelect(&menu, &processID); break;
            case MENU_ADD_PROCESS_SELF: menuAddProcessSelf(&menu, &processID); break;
            case MENU_ADD_PROCESS_BROWSE: menuAddProcessBrowse(&menu, &processID); break;
            case MENU_ADD_PROCESS_NEWS: menuAddProcessNews(&menu, &processID); break;
            case MENU_ADD_NEWS: menuAddNews(&menu, title, message, image, &imgSize, &hasImage, &processID); break;
            default: break;
        }
        
        //break loop
        if (menu==MENU_EXIT) break;
    }
    
    //clearup
    free(image);
    consoleSelect(&top);
    consoleClear();
    consoleSelect(&bot);
    consoleClear();
    
    //finalization
    gfxExit();
    amExit();
    fsExit();
    aptExit();
    newsExit();
    return 0;
}