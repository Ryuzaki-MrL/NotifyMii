#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "keyboard.h"
#include "notification.h"
#include "utils.h"

PrintConsole top;
PrintConsole bot;
u8 menu = 0;
u32 selected = 0;
u32 scroll = 0;
u32 total;
char title[32];
char message[0x1780];
u32 offset = 0;
u8 phase = 0;
bool shifted = false;

void openMenu(u8 m) {
    selected = 0;
    scroll = 0;
    phase = 0;
    offset = 0;
    memset(&title[0], 0, sizeof(title));
    memset(&message[0], 0, sizeof(message));
    shifted = false;
    menu = m;
    consoleSelect(&top);
    consoleClear();
    printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.0                                    \x1b[0m");
    if (menu==0) printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select                       \x1b[0m");
    if (menu==2) printf("\x1b[29;0H\x1b[47;30mA: Read / Y: Dump / X: Delete / B: Back           \x1b[0m");
    consoleSelect(&bot);
    consoleClear();
}

int main(int argc, char **argv)
{
    //initialization
    newsInit();
    fsInit();
    gfxInitDefault();
	consoleInit(GFX_TOP, &top);
	consoleInit(GFX_BOTTOM, &bot);
	consoleSelect(&top);
    
    //create NotifyMii folder
    FS_Archive sdmcArchive = (FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
    FS_Path path = fsMakePath(PATH_ASCII, "NotifyMii");
    FSUSER_CreateDirectory(sdmcArchive, path, 0);
    
    //create news list
    NewsList* news = createVoice(0);
    NewsList* list = news;
    
    //get total notifications
    NEWS_GetTotalNotifications(&total);
    
    //get all notification titles
    printf("\x1b[0;0HLoading %lu notifications...", total);
    
    u32 i = 1;
    
	while (i < total) {
		NewsList* voice = createVoice(i);
		list->next = voice;
		list = list->next;
		i++;
	}
    
    //open main menu
    openMenu(0);
    
    //main loop
    while ( aptMainLoop() )
    {
        //if main menu
        if (menu==0)
        {
            //scan input
            hidScanInput();
            u32 kDown = hidKeysDown();
            
            //print options
            printf("\x1b[%lu;0H \n>\n ", selected);
            printf("\x1b[0;1HMAIN MENU");
            printf("\x1b[1;2HAdd Notification");
            printf("\x1b[2;2HView Notifications");
            printf("\x1b[3;2HClear Notifications");
            printf("\x1b[4;2HExit");
            
            printf("\x1b[29;0HFound %lu notifications.", total);
            
            //move cursor
            if (kDown & KEY_DOWN) {
                if (selected<3) selected++;
            }
            if (kDown & KEY_UP) {
                if (selected>0) selected--;
            }
            
            //open selected menu
            if (kDown & KEY_A) {
                openMenu(1 + selected);
                if (menu==1) printKeyboard();
                if (menu==2) printNews(news);
            }
            
            //ends loop
            if (kDown & KEY_START) break;
        }
        
        //if notification add menu
        if (menu==1)
        {
            //scan input
            hidScanInput();
            u32 kDown = hidKeysDown();
            
            //print title and message
            printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.0                                    \x1b[0m");
            printf("\x1b[28;0H\x1b[47;30mD-PAD: Move / A: Input / B: Back / L: Backspace / \x1b[29;0HSELECT: Shift / START: Confirm                    \x1b[0m");
            if (phase==0) printf("\x1b[1;0HTitle:\x1b[2;0H%s", title);
            if (phase==1) printf("\x1b[1;0HMessage:\x1b[2;0H%s", message);
            if (phase==2) {
                printf("\x1b[%lu;0H \n>\n ", 1 + selected);
                printf("\x1b[2;2HUse default.jpg");
                printf("\x1b[3;2HUse notification image");
                printf("\x1b[4;2HDon't use an image");
            }
            
            //shift keyboard
            if (kDown & KEY_SELECT) {
                shifted=(!shifted);
                printKeyboard();
            }
            
            //move cursor
            if (kDown & KEY_LEFT) {
                if (phase < 2) {
                    if ( (selected % 12) > 0 ) selected--;
                    printKeyboard();
                }
            }
            if (kDown & KEY_RIGHT) {
                if (phase < 2) {
                    if ( (selected % 12) < 11 ) selected++;
                    printKeyboard();
                }
            }
            if (kDown & KEY_UP) {
                if (phase < 2) {
                    if (selected > 11) selected-=12;
                    printKeyboard();
                }
                else {
                    if (selected > 0) selected--;
                }
            }
            if (kDown & KEY_DOWN) {
                if (phase < 2) {
                    if (selected < 36) selected+=12;
                    printKeyboard();
                }
                else {
                    if (selected < 2) selected++;
                }
                
            }
            
            //add char
            if (kDown & KEY_A) {
                if (phase==0) {
                    if (offset < 31) { title[offset] = getKeyboardChar(selected); offset++; }
                }
                if (phase==1) {
                    if (offset < 0x1779) { message[offset] = getKeyboardChar(selected); offset++; }
                }
                if (phase==2) {
                    addNews(title, message, selected, news);
                    openMenu(0);
                }
            }
            
            //delete char
            if (kDown & KEY_L) {
                if (offset > 0) offset--;
                if (phase==0) title[offset] = '\0';
                if (phase==1) message[offset] = '\0';
                consoleClear();
            }
            
            //confirm and continue
            if (kDown & KEY_START) {
                if (phase<2) { phase++; offset = 0; }
                if (phase==2) selected = 0;
                consoleClear();
            }
            
            //return to main menu
            if (kDown & KEY_B) openMenu(0);
        }
        
        //if notification list menu
        if (menu==2)
        {
            //scan input
            hidScanInput();
            u32 kDown = hidKeysDown();
            
            //returns to main menu if there are no news
            if (total==0) {
                printf("\x1b[0;0HNo notifications found.");
                waitKey();
                openMenu(0);
            }
            
            //move cursor
            if (kDown & KEY_DOWN) {
                if (selected<14) selected++;
                else if ( (selected + scroll) < (total - 15) ) scroll++;
                else if (selected<28) selected++;
                else { selected = 0; scroll = 0; }
                printNews(news);
            }
            if (kDown & KEY_UP) {
                if (selected>13) selected--;
                else if (scroll>0) scroll--;
                else if (selected>0) selected--;
                else {
                    if (total>28) { selected = 28; scroll = total - 29; }
                    else if (total>0) selected = total - 1;
                }
                printNews(news);
            }
            
            //read selected news
            if (kDown & KEY_A) {
                readNews(selected + scroll, news);
                printNews(news);
            }
            
            //return to main menu
            if (kDown & KEY_B) openMenu(0);
            
            //delete selected news
            if (kDown & KEY_X) {
                news = deleteNews(selected + scroll, news);
                printNews(news);
            }
            
            //dump selected news
            if (kDown & KEY_Y) {
                dumpNews(selected + scroll, news);
            }
        }
        
        //if notification clear menu
        if (menu==3)
        {
            //delete all notifications
            clearNews(news);
            openMenu(0);
        }
        
        //ends main loop
        if (menu==4) break;
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    
    //clears current console
    consoleClear();
    
    //delete news list
    NewsList* tmp = news;
	while (tmp != NULL) {
		news = news->next;
		free(tmp);
		tmp = news;
	}
    
    //finalization
    gfxExit();
    fsExit();
    newsExit();
    return 0;
}