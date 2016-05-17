#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack>
#include <vector>
#include <algorithm>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>

#include "keyboard.h"
#include "menu.h"
#include "notification.h"
#include "utils.h"

void drawImage(u8 *imgBuffer, u32 imgSize) {
    gfxExit();
    sf2d_init();
    sf2d_set_3D(0);
    sf2d_texture *image = sfil_load_JPEG_buffer(imgBuffer, imgSize, SF2D_PLACE_RAM);
    while ( aptMainLoop() ) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown) break;
        sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_texture(image, 0, 0);
        sf2d_end_frame();
        sf2d_swapbuffers();
    }
    sf2d_free_texture(image);
    sf2d_fini();
    gfxInitDefault();
    consoleInit(GFX_TOP, &top);
    consoleInit(GFX_BOTTOM, &bot);
}


bool isDirectory(std::string path) {
    bool result = false;
    DIR *dir = opendir(path.c_str());
    if (dir != NULL) result = true;
    closedir(dir);
    return result;
}


void sortFileList(std::vector<entry> *filelist) {
    struct alphabetically {
        inline bool operator() (entry a, entry b) {
            if(a.isDir == b.isDir)
                return strcasecmp(a.name.c_str(), b.name.c_str()) < 0;
            else return a.isDir;
        }
    } sort_a;
    std::sort((*filelist).begin(), (*filelist).end(), sort_a);
}


std::vector<entry> getFileList(std::string directory, std::string extension) {
    std::vector<entry> result;
    DIR* dir = opendir(directory.c_str());
    if(dir == NULL) return result;
    
    dirent* ent = NULL;
    do {
        ent = readdir(dir);
        if (ent != NULL) {
            std::string file(ent->d_name);
            bool isDir = isDirectory(directory + file + "/");
            std::string::size_type dotPos = file.rfind('.');
            if( (extension == file.substr(dotPos+1)) || ( isDir ) ) result.push_back({file, isDir});
        }
    } while(ent != NULL);
    
    closedir(dir);
    sortFileList(&result);
    return result;
}


void printFiles(u32 selected, u32 scroll, u32 count, std::vector<entry> *files, std::string curdir) {
    consoleSelect(&top);
    consoleClear();
    printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.2                                    \x1b[0m");
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
    consoleSelect(&bot);
    consoleClear();
    bool isRoot = (curdir=="/");
    printf("\x1b[0;0H%.40s", curdir.c_str());
    printf("\x1b[%lu;0H>", 1 + selected);
    u32 i = 0;
    while (i < count) {
        if (i > 27) break;
        if ( (*files)[i+scroll].isDir ) printf("\x1b[%lu;2H\x1b[33m%.38s\x1b[0m", 2 + i, (*files)[i+scroll].name.c_str());
        else printf("\x1b[%lu;2H%.38s", 2 + i, (*files)[i+scroll].name.c_str());
        i++;
    }
    if (isRoot) printf("\x1b[1;2H\x1b[35m[root]\x1b[0m");
    else printf("\x1b[1;2H\x1b[37m..\x1b[0m");
}


void printTitles(u32 selected, u32 scroll, u32 count, std::vector<u64> *titles, FS_MediaType media) {
    consoleSelect(&bot);
    consoleClear();
    if (media==MEDIATYPE_SD) printf("\x1b[0;0H/SD");
    else printf("\x1b[0;0H/NAND");
    printf("\x1b[%lu;0H>", 1 + selected);
    u32 i = 0;
    while (i < count) {
        if (i > 28) break;
        printf("\x1b[%lu;2H%#llx", 1 + i, (*titles)[i+scroll]);
        i++;
    }
}


void menuMain(u8 *menu) {
    u8 selected = 0;
    
    u32 total;
    NEWS_GetTotalNotifications(&total);
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select                       \x1b[0m");
    
    consoleSelect(&bot);
    printf("\x1b[0;1HMAIN MENU");
    printf("\x1b[1;2HAdd Notification");
    printf("\x1b[2;2HView Notifications");
    printf("\x1b[3;2HClear Notifications");
    printf("\x1b[4;2HExit");
    
    printf("\x1b[29;0HFound %lu notifications.", total);
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //print cursor
        printf("\x1b[%u;0H \n>\n ", selected);
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (selected<3) selected++;
        }
        if (kDown & KEY_UP) {
            if (selected>0) selected--;
        }
        
        //open selected menu
        if (kDown & KEY_A) {
            *menu = 1 + selected;
            break;
        }
        
        //ends loop
        if (kDown & KEY_START) {
            *menu = MENU_EXIT;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddTitle(u8 *menu, char *title) {
    char buffer[32];
    memcpy(buffer, title, 32);
    u32 offset = strlen(buffer);
    bool shift = false;
    u8 selected = 0;
    
    consoleSelect(&top);
    printf("\x1b[28;0H\x1b[47;30mD-PAD: Move / A: Input / B: Back / L: Backspace / \x1b[29;0HSELECT: Shift / START: Confirm                    \x1b[0m");
    printf("\x1b[1;0HNotification Title:");
    
    printKeyboard(selected, shift);
    printf("\x1b[2;0H%s ", buffer);
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //shift keyboard
        if (kDown & KEY_SELECT) {
            shift=(!shift);
            printKeyboard(selected, shift);
        }
        
        //move cursor
        if (kDown & KEY_LEFT) {
            if ( (selected % 12) > 0 ) selected--;
            printKeyboard(selected, shift);
        }
        if (kDown & KEY_RIGHT) {
            if ( (selected % 12) < 11 ) selected++;
            printKeyboard(selected, shift);
        }
        if (kDown & KEY_UP) {
            if (selected > 11) selected-=12;
            printKeyboard(selected, shift);
        }
        if (kDown & KEY_DOWN) {
            if (selected < 36) selected+=12;
            printKeyboard(selected, shift);
        }
        
        //add char
        if (kDown & KEY_A) {
            if (offset < 31) {
                buffer[offset] = getKeyboardChar(selected, shift);
                offset++;
                printf("\x1b[2;0H%s ", buffer);
            }
        }
        
        //delete char
        if (kDown & KEY_L) {
            if (offset > 0) offset--;
            buffer[offset] = '\0';
            printf("\x1b[2;0H%s ", buffer);
        }
        
        //confirm and continue
        if (kDown & KEY_START) {
            memcpy(title, buffer, 32);
            *menu = MENU_ADD_MESSAGE_SELECT;
            break;
        }
        
        //return to main menu
        if (kDown & KEY_B) {
            *menu = MENU_MAIN;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuList(u8 *menu) {
    u32 selected = 0;
    u32 scroll = 0;
    
    u32 total;
    NEWS_GetTotalNotifications(&total);
    
    consoleSelect(&top);
    printf("\x1b[28;0H\x1b[47;30mA: Read / Y: Dump / X: Delete / R: Image / B: Back\x1b[0m");
    printf("\x1b[29;0H\x1b[47;30mSELECT: Dump all / START: Launch app              \x1b[0m");
    
    printNews(selected, scroll, true);
    
    while ( aptMainLoop() )
    {
        //break if there are no notifications
        if (total==0) {
            printf("\x1b[0;0HYou don't have any notifications!\nPress any key to continue.");
            waitKey();
            *menu = MENU_MAIN;
            break;
        }
        
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (total < 29) {
                if (selected < ( total - 1) ) selected++;
                else selected=0;
            }
            else if (total > 0) {
                if (selected<14) selected++;
                else if ( (selected + scroll) < (total - 15) ) scroll++;
                else if (selected<28) selected++;
                else { selected = 0; scroll = 0; }
            }
            printNews(selected, scroll, true);
        }
        if (kDown & KEY_UP) {
            if (selected>13) selected--;
            else if (scroll>0) scroll--;
            else if (selected>0) selected--;
            else {
                if (total>28) { selected = 28; scroll = total - 29; }
                else if (total>0) selected = total - 1;
            }
            printNews(selected, scroll, true);
        }
        
        //read news
        if (kDown & KEY_A) {
            readNews(selected + scroll);
            printNews(selected, scroll, false);
        }
        
        //view image
        if (kDown & KEY_R) {
            u32 size;
            NotificationHeader header;
            NEWS_GetNotificationHeader(selected + scroll, &header);
            if (header.enableJPEG) {
                u8* buffer = (u8*)malloc(0x20000);
                NEWS_GetNotificationImage(selected + scroll, buffer, &size);
                drawImage(buffer, size);
                free(buffer);
                printNews(selected, scroll, true);
            }
        }
        
        //dump news
        if (kDown & KEY_Y) {
            dumpNews(selected + scroll);
            consoleSelect(&top);
            printf("\x1b[27;0HDumped 1 notification to 'SD:/NotifyMii'.");
            
        }
        
        //dump all
        if (kDown & KEY_SELECT) {
            u32 i = 0;
            while (i < total) {
                dumpNews(i);
                i++;
            }
            consoleSelect(&top);
            printf("\x1b[26;0HDumped %lu notifications to 'SD:/NotifyMii'.\nPress any key to continue.", total);
            waitKey();
        }
        
        //delete news
        if (kDown & KEY_X) {
            deleteNews(selected + scroll);
            NEWS_GetTotalNotifications(&total);
            if (scroll>0) scroll--;
            else if (selected>0) selected--;
            printNews(selected, scroll, true);
        }
        
        //launch process
        if (kDown & KEY_START) {
            NotificationHeader header;
            NEWS_GetNotificationHeader(selected + scroll, &header);
            if (header.processID > 0) {
                u32 count;
                u8 media = MEDIATYPE_SD;
                AM_GetTitleCount(MEDIATYPE_SD, &count);
                u64 *ids = new u64[count];
                AM_GetTitleList(&count, MEDIATYPE_SD, count, ids);
                u32 i = 0;
                while ( (ids[i] != header.processID) && (i < count) ) {
                    i++;
                }
                delete[] ids;
                if (i==count) {
                    i = 0;
                    media = MEDIATYPE_NAND;
                    u32 count;
                    AM_GetTitleCount(MEDIATYPE_NAND, &count);
                    u64 *ids = new u64[count];
                    AM_GetTitleList(&count, MEDIATYPE_NAND, count, ids);
                    u32 i = 0;
                    while ( (ids[i] != header.processID) && (i < count) ) {
                        i++;
                    }
                    delete[] ids;
                    if (i==count) {
                        consoleSelect(&top);
                        printf("\x1b[26;0HTitle not found.\nPress any key to continue.");
                        waitKey();
                        break;
                    }
                }
                u8 buf0[0x300];
                u8 buf1[0x20];
                memset(buf0, 0, 0x300);
                memset(buf1, 0, 0x20);
                aptOpenSession();
                APT_PrepareToDoAppJump(0, header.processID, media);
                APT_DoAppJump(0x300, 0x20, buf0, buf1);
                aptCloseSession();
            }
        }
        
        //return to previous menu
        if (kDown & KEY_B) {
            *menu = MENU_MAIN;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuClear(u8 *menu) {
    u32 total;
    u32 deleted = 0;
	NotificationHeader header = { 0 };
    NEWS_GetTotalNotifications(&total);
    u32 i = 0;
	while (i < total) {
		Result ret = NEWS_SetNotificationHeader(i, (const NotificationHeader*)&header);
        if (!ret) deleted++;
        i++;
	}
    consoleSelect(&top);
	printf("\x1b[1;0H%lu notifications deleted!\nPress any key to continue.", deleted);
    waitKey();
    *menu = MENU_MAIN;
}


void menuAddMessageSelect(u8 *menu) {
    u8 selected = 0;
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / B: Back             \x1b[0m");
    
    consoleSelect(&bot);
    printf("\x1b[0;1HSELECT MESSAGE SOURCE:");
    printf("\x1b[1;2HWrite a message");
    printf("\x1b[2;2HUse a text file");
    printf("\x1b[3;2HSelect from notifications");
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //print cursor
        printf("\x1b[%u;0H \n>\n ", selected);
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (selected<2) selected++;
        }
        if (kDown & KEY_UP) {
            if (selected>0) selected--;
        }
        
        //open selected menu
        if (kDown & KEY_A) {
            *menu = MENU_ADD_MESSAGE_KEYBOARD + selected;
            break;
        }
        
        //ends loop
        if (kDown & KEY_B) {
            *menu = MENU_ADD_TITLE;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddMessageKeyboard(u8 *menu, char *message) {
    char buffer[0x1780];
    memcpy(buffer, message, 0x1780);
    u32 offset = strlen(buffer);
    bool shift = false;
    u8 selected = 0;
    
    consoleSelect(&top);
    printf("\x1b[28;0H\x1b[47;30mD-PAD: Move / A: Input / B: Back / L: Backspace / \x1b[29;0HSELECT: Shift / START: Confirm                    \x1b[0m");
    printf("\x1b[1;0HNotification Message:");
    
    printKeyboard(selected, shift);
    printf("\x1b[2;0H%s ", buffer);
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //scan touch
        //touchPosition touch;
        //hidTouchRead(&touch);
        //printf("\x1b[27;0HTouch: %03d %03d", touch.px, touch.py);
        
        //shift keyboard
        if (kDown & KEY_SELECT) {
            shift=(!shift);
            printKeyboard(selected, shift);
        }
        
        //move cursor
        if (kDown & KEY_LEFT) {
            if ( (selected % 12) > 0 ) selected--;
            printKeyboard(selected, shift);
        }
        if (kDown & KEY_RIGHT) {
            if ( (selected % 12) < 11 ) selected++;
            printKeyboard(selected, shift);
        }
        if (kDown & KEY_UP) {
            if (selected > 11) selected-=12;
            printKeyboard(selected, shift);
        }
        if (kDown & KEY_DOWN) {
            if (selected < 36) selected+=12;
            printKeyboard(selected, shift);
        }
        
        //add char
        if (kDown & KEY_A) {
            if (offset < 0x1780) {
                buffer[offset] = getKeyboardChar(selected, shift);
                offset++;
                printf("\x1b[2;0H%s ", buffer);
            }
        }
        
        //delete char
        if (kDown & KEY_L) {
            if (offset > 0) offset--;
            buffer[offset] = '\0';
            printf("\x1b[2;0H%s ", buffer);
        }
        
        //confirm and continue
        if (kDown & KEY_START) {
            memcpy(message, buffer, 0x1780);
            *menu = MENU_ADD_IMAGE_SELECT;
            break;
        }
        
        //return to previous menu
        if (kDown & KEY_B) {
            *menu = MENU_ADD_MESSAGE_SELECT;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddMessageFile(u8 *menu, char *message) {
    u32 selected = 0;
    u32 scroll = 0;
    std::string curdir = "/";
    std::stack<std::string> innerpath;
    std::vector<entry> filelist = getFileList(curdir, "txt");
    u32 count = filelist.size();
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
    
    printFiles(selected, scroll, count, &filelist, curdir);
    
    while ( aptMainLoop() ) {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (count < 28) {
                if (selected < ( count ) ) selected++;
                else selected=0;
            }
            else if (count > 0) {
                if (selected<14) selected++;
                else if ( (selected + scroll) < (count - 14) ) scroll++;
                else if (selected<28) selected++;
                else { selected = 0; scroll = 0; }
            }
            printFiles(selected, scroll, count, &filelist, curdir);
        }
        if (kDown & KEY_UP) {
            if (selected>13) selected--;
            else if (scroll>0) scroll--;
            else if (selected>0) selected--;
            else {
                if (count>27) { selected = 28; scroll = count - 28; }
                else if (count>0) selected = count;
            }
            printFiles(selected, scroll, count, &filelist, curdir);
        }
        
        //select file
        if (kDown & KEY_A) {
            if (selected > 0) {
                if ( filelist[selected+scroll-1].isDir ) {
                    innerpath.push(curdir);
                    curdir = curdir + filelist[selected+scroll-1].name + "/";
                    std::vector<entry> newlist = getFileList(curdir, "txt");
                    filelist.swap(newlist);
                    selected = 0; scroll = 0; count = filelist.size();
                    printFiles(selected, scroll, count, &filelist, curdir);
                }
                else {
                    u32 bytes;
                    Handle fileHandle;
                    FS_Archive sdmcArchive=(FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
                    std::string path = curdir + filelist[selected+scroll-1].name;
                    FS_Path filePath=fsMakePath(PATH_ASCII, path.c_str());
                    Result res = FSUSER_OpenFileDirectly( &fileHandle, sdmcArchive, filePath, FS_OPEN_READ, 0x00000000);
                    if (res) {
                        printf("\x1b[1;0HCouldn't open %s.\nPress any key to continue.", filelist[selected+scroll-1].name.c_str());
                        waitKey();
                    }
                    else {
                        u64 size;
                        FSFILE_GetSize(fileHandle, &size);
                        if (size > 0x1780) {
                            consoleSelect(&top);
                            printf("\x1b[1;0HText file too large.\nPress any key to continue.");
                            waitKey();
                        }
                        else {
                            memset(message, '\0', 0x1780);
                            FSFILE_Read(fileHandle, &bytes, 0, message, size);
                            FSFILE_Close(fileHandle);
                            *menu = MENU_ADD_IMAGE_SELECT;
                            svcCloseHandle(fileHandle);
                            break;
                        }
                    }
                    svcCloseHandle(fileHandle);
                }
            }
            else {
                if (curdir!="/") {
                    curdir = innerpath.top();
                    innerpath.pop();
                    std::vector<entry> newlist = getFileList(curdir, "txt");
                    filelist.swap(newlist);
                    selected = 0; scroll = 0; count = filelist.size();
                    printFiles(selected, scroll, count, &filelist, curdir);
                }
            }
        }
        
        //preview file
        if (kDown & KEY_Y) {
            if (selected > 0) {
                if ( !filelist[selected+scroll-1].isDir ) {
                    u32 bytes;
                    Handle fileHandle;
                    FS_Archive sdmcArchive=(FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
                    std::string path = curdir + filelist[selected+scroll-1].name;
                    FS_Path filePath=fsMakePath(PATH_ASCII, path.c_str());
                    Result res = FSUSER_OpenFileDirectly( &fileHandle, sdmcArchive, filePath, FS_OPEN_READ, 0x00000000);
                    if (res) {
                        printf("\x1b[1;0HCouldn't open %s.\nPress any key to continue.", filelist[selected+scroll-1].name.c_str());
                        waitKey();
                    }
                    else {
                        u64 size;
                        FSFILE_GetSize(fileHandle, &size);
                        if (size > 0x1780) {
                            consoleSelect(&top);
                            printf("\x1b[1;0HText file too large.\nPress any key to continue.");
                            waitKey();
                        }
                        else {
                            memset(message, '\0', 0x1780);
                            FSFILE_Read(fileHandle, &bytes, 0, message, size);
                            FSFILE_Close(fileHandle);
                            consoleSelect(&top);
                            consoleClear();
                            printf("\x1b[1;0H%s", message);
                            printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.2                                    \x1b[0m");
                            printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
                        }
                    }
                    svcCloseHandle(fileHandle);
                }
            }
        }
        
        //return to previous menu
        if (kDown & KEY_B) {
            if (curdir=="/") {
                memset(message, '\0', 0x1780);
                *menu = MENU_ADD_MESSAGE_SELECT;
                break;
            }
            else if (!innerpath.empty()) {
                curdir = innerpath.top();
                innerpath.pop();
                std::vector<entry> newlist = getFileList(curdir, "txt");
                filelist.swap(newlist);
                selected = 0; scroll = 0; count = filelist.size();
                printFiles(selected, scroll, count, &filelist, curdir);
            }
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddMessageNews(u8 *menu, char *message) {
    u32 selected = 0;
    u32 scroll = 0;
    
    u32 total;
    NEWS_GetTotalNotifications(&total);
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
    
    printNews(selected, scroll, false);
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (total < 29) {
                if (selected < ( total - 1) ) selected++;
                else selected=0;
            }
            else if (total > 0) {
                if (selected<14) selected++;
                else if ( (selected + scroll) < (total - 15) ) scroll++;
                else if (selected<28) selected++;
                else { selected = 0; scroll = 0; }
            }
            printNews(selected, scroll, false);
        }
        if (kDown & KEY_UP) {
            if (selected>13) selected--;
            else if (scroll>0) scroll--;
            else if (selected>0) selected--;
            else {
                if (total>28) { selected = 28; scroll = total - 29; }
                else if (total>0) selected = total - 1;
            }
            printNews(selected, scroll, false);
        }
        
        //select news
        if (kDown & KEY_A) {
            u16 tmp[0x1780];
            NEWS_GetNotificationMessage(selected + scroll, tmp);
            utf2ascii(message, tmp);
            *menu = MENU_ADD_IMAGE_SELECT;
            break;
        }
        
        //preview news
        if (kDown & KEY_Y) {
            u16 tmp[0x1780];
            NEWS_GetNotificationMessage(selected + scroll, tmp);
            utf2ascii(message, tmp);
            consoleSelect(&top);
            consoleClear();
            printf("\x1b[1;0H%s", message);
            printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.2                                    \x1b[0m");
            printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
            printNews(selected, scroll, false);
        }
        
        //return to previous menu
        if (kDown & KEY_B) {
            memset(message, '\0', 0x1780);
            *menu = MENU_ADD_MESSAGE_SELECT;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddImageSelect(u8 *menu) {
    u8 selected = 0;
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / B: Back             \x1b[0m");
    
    consoleSelect(&bot);
    printf("\x1b[0;1HSELECT IMAGE SOURCE:");
    printf("\x1b[1;2HUse 'default.jpg'");
    printf("\x1b[2;2HSelect from SD card");
    printf("\x1b[3;2HSelect from notifications");
    printf("\x1b[4;2HDon't use an image");
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //print cursor
        printf("\x1b[%u;0H \n>\n ", selected);
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (selected<3) selected++;
        }
        if (kDown & KEY_UP) {
            if (selected>0) selected--;
        }
        
        //open selected menu
        if (kDown & KEY_A) {
            *menu = MENU_ADD_IMAGE_DEFAULT + selected;
            break;
        }
        
        //ends loop
        if (kDown & KEY_B) {
            *menu = MENU_ADD_MESSAGE_SELECT;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddImageDefault(u8 *menu, u8 *image, u32 *imgSize) {
    consoleSelect(&top);
    Handle imgHandle;
    FS_Archive sdmcArchive = (FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
    char filePath[] = "/NotifyMii/default.jpg";
    FS_Path imgPath = fsMakePath(PATH_ASCII, filePath);
    Result res = FSUSER_OpenFileDirectly( &imgHandle, sdmcArchive, imgPath, FS_OPEN_READ, 0x00000000);
    if (res) {
        printf("\x1b[1;0HCouldn't open 'default.jpg'.\nPress any key to continue.");
        waitKey();
        *menu = MENU_ADD_IMAGE_SELECT;
    }
    else {
        u64 size;
        FSFILE_GetSize(imgHandle, &size);
        if (size > 0xC800) {
            consoleSelect(&top);
            printf("\x1b[1;0HImage file too large.\nPress any key to continue.");
            waitKey();
            *menu = MENU_ADD_MESSAGE_SELECT;
        }
        else {
            FSFILE_Read(imgHandle, imgSize, 0, image, size);
            FSFILE_Close(imgHandle);
            *menu = MENU_ADD_PROCESS_SELECT;
        }
    }
    svcCloseHandle(imgHandle);
}


void menuAddImageFile(u8 *menu, u8 *image, u32 *imgSize) {
    u32 selected = 0;
    u32 scroll = 0;
    std::string curdir = "/";
    std::stack<std::string> innerpath;
    std::vector<entry> filelist = getFileList(curdir, "jpg");
    u32 count = filelist.size();
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
    
    printFiles(selected, scroll, count, &filelist, curdir);
    
    while ( aptMainLoop() ) {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (count < 28) {
                if (selected < ( count ) ) selected++;
                else selected=0;
            }
            else if (count > 0) {
                if (selected<14) selected++;
                else if ( (selected + scroll) < (count - 14) ) scroll++;
                else if (selected<28) selected++;
                else { selected = 0; scroll = 0; }
            }
            printFiles(selected, scroll, count, &filelist, curdir);
        }
        if (kDown & KEY_UP) {
            if (selected>13) selected--;
            else if (scroll>0) scroll--;
            else if (selected>0) selected--;
            else {
                if (count>27) { selected = 28; scroll = count - 28; }
                else if (count>0) selected = count;
            }
            printFiles(selected, scroll, count, &filelist, curdir);
        }
        
        //select file
        if (kDown & KEY_A) {
            if (selected > 0) {
                if ( filelist[selected+scroll-1].isDir ) {
                    innerpath.push(curdir);
                    curdir = curdir + filelist[selected+scroll-1].name + "/";
                    std::vector<entry> newlist = getFileList(curdir, "jpg");
                    filelist.swap(newlist);
                    selected = 0; scroll = 0; count = filelist.size();
                    printFiles(selected, scroll, count, &filelist, curdir);
                }
                else {
                    Handle fileHandle;
                    FS_Archive sdmcArchive=(FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
                    std::string path = curdir + filelist[selected+scroll-1].name;
                    FS_Path filePath=fsMakePath(PATH_ASCII, path.c_str());
                    Result res = FSUSER_OpenFileDirectly( &fileHandle, sdmcArchive, filePath, FS_OPEN_READ, 0x00000000);
                    if (res) {
                        printf("\x1b[1;0HCouldn't open %s.\nPress any key to continue.", filelist[selected+scroll-1].name.c_str());
                        waitKey();
                    }
                    else {
                        u64 size;
                        FSFILE_GetSize(fileHandle, &size);
                        if (size > 0xC800) {
                            consoleSelect(&top);
                            printf("\x1b[1;0HImage file too large.\nPress any key to continue.");
                            waitKey();
                        }
                        else {
                            FSFILE_Read(fileHandle, imgSize, 0, image, size);
                            FSFILE_Close(fileHandle);
                            *menu = MENU_ADD_PROCESS_SELECT;
                            svcCloseHandle(fileHandle);
                            break;
                        }
                    }
                    svcCloseHandle(fileHandle);
                }
            }
            else {
                if (curdir!="/") {
                    curdir = innerpath.top();
                    innerpath.pop();
                    std::vector<entry> newlist = getFileList(curdir, "jpg");
                    filelist.swap(newlist);
                    selected = 0; scroll = 0; count = filelist.size();
                    printFiles(selected, scroll, count, &filelist, curdir);
                }
            }
        }
        
        //preview image
        if (kDown & KEY_Y) {
            if (selected > 0) {
                if ( !filelist[selected+scroll-1].isDir ) {
                    Handle fileHandle;
                    FS_Archive sdmcArchive=(FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
                    std::string path = curdir + filelist[selected+scroll-1].name;
                    FS_Path filePath=fsMakePath(PATH_ASCII, path.c_str());
                    Result res = FSUSER_OpenFileDirectly( &fileHandle, sdmcArchive, filePath, FS_OPEN_READ, 0x00000000);
                    if (res) {
                        printf("\x1b[1;0HCouldn't open %s.\nPress any key to continue.", filelist[selected+scroll-1].name.c_str());
                        waitKey();
                    }
                    else {
                        u64 size;
                        FSFILE_GetSize(fileHandle, &size);
                        if ( (size > 0xC800) ) {
                            consoleSelect(&top);
                            printf("\x1b[1;0HImage file too large.\nPress any key to continue.");
                            waitKey();
                        }
                        else if (size > 0) {
                            u8* buffer = (u8*)malloc(0xC800);
                            u32 bufferSize;
                            FSFILE_Read(fileHandle, &bufferSize, 0, buffer, size);
                            FSFILE_Close(fileHandle);
                            drawImage(buffer, bufferSize);
                            free(buffer);
                            consoleSelect(&top);
                            printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.2                                    \x1b[0m");
                            printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
                            printFiles(selected, scroll, count, &filelist, curdir);
                        }
                        else {
                            consoleSelect(&top);
                            printf("\x1b[1;0HInvalid file.\nPress any key to continue.");
                            waitKey();
                        }
                    }
                    svcCloseHandle(fileHandle);
                }
            }
        }
        
        //return to previous menu
        if (kDown & KEY_B) {
            if (curdir=="/") {
                *menu = MENU_ADD_IMAGE_SELECT;
                break;
            }
            else if (!innerpath.empty()){
                curdir = innerpath.top();
                innerpath.pop();
                std::vector<entry> newlist = getFileList(curdir, "jpg");
                filelist.swap(newlist);
                selected = 0; scroll = 0; count = filelist.size();
                printFiles(selected, scroll, count, &filelist, curdir);
            }
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddImageNews(u8 *menu, u8 *image, u32 *imgSize) {
    u32 selected = 0;
    u32 scroll = 0;
    
    u32 total;
    NEWS_GetTotalNotifications(&total);
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
    
    printNews(selected, scroll, false);
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (total < 29) {
                if (selected < ( total - 1) ) selected++;
                else selected=0;
            }
            else if (total > 0) {
                if (selected<14) selected++;
                else if ( (selected + scroll) < (total - 15) ) scroll++;
                else if (selected<28) selected++;
                else { selected = 0; scroll = 0; }
            }
            printNews(selected, scroll, false);
        }
        if (kDown & KEY_UP) {
            if (selected>13) selected--;
            else if (scroll>0) scroll--;
            else if (selected>0) selected--;
            else {
                if (total>28) { selected = 28; scroll = total - 29; }
                else if (total>0) selected = total - 1;
            }
            printNews(selected, scroll, false);
        }
        
        //select news
        if (kDown & KEY_A) {
            NotificationHeader header;
            NEWS_GetNotificationHeader(selected + scroll, &header);
            if (header.enableJPEG) {
                Result res = NEWS_GetNotificationImage(selected + scroll, image, imgSize);
                if (res) {
                    consoleSelect(&top);
                    printf("\x1b[1;0HFailed to get notification image.\nPress any key to continue.");
                    waitKey();
                    *menu = MENU_ADD_IMAGE_SELECT;
                    break;
                }
                else {
                    *menu = MENU_ADD_PROCESS_SELECT;
                    break;
                }
            }
            else {
                consoleSelect(&top);
                printf("\x1b[1;0HNotification has no image.\nPress any key to continue.");
                waitKey();
                *menu = MENU_ADD_IMAGE_SELECT;
                break;
            }
        }
        
        //preview image
        if (kDown & KEY_Y) {
            u32 size;
            NotificationHeader header;
            NEWS_GetNotificationHeader(selected + scroll, &header);
            if (header.enableJPEG) {
                u8* buffer = (u8*)malloc(0xC800);
                NEWS_GetNotificationImage(selected + scroll, buffer, &size);
                drawImage(buffer, size);
                free(buffer);
                consoleSelect(&top);
                printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.2                                    \x1b[0m");
                printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m");
                printNews(selected, scroll, false);
            }
        }
        
        //return to previous menu
        if (kDown & KEY_B) {
            *menu = MENU_ADD_IMAGE_SELECT;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddImageEmpty(u8 *menu, u8 *image, u32 *imgSize, bool *hasImage) {
    memset(image, 0, 0xC800);
    *imgSize = 0xC800;
    *hasImage = false;
    *menu = MENU_ADD_PROCESS_SELECT;
}


void menuAddProcessSelect(u8 *menu, u64 *processID) {
    u8 selected = 0;
    *processID = 0;
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / B: Back             \x1b[0m");
    
    consoleSelect(&bot);
    printf("\x1b[0;1HSELECT NOTIFICATION ORIGIN PROCESS:");
    printf("\x1b[1;2HUse NotifyMii");
    printf("\x1b[2;2HSelect from installed titles");
    printf("\x1b[3;2HSelect from notifications");
    printf("\x1b[4;2HDon't use any title");
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //print cursor
        printf("\x1b[%u;0H \n>\n ", selected);
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (selected<3) selected++;
        }
        if (kDown & KEY_UP) {
            if (selected>0) selected--;
        }
        
        //open selected menu
        if (kDown & KEY_A) {
            *menu = MENU_ADD_PROCESS_SELF + selected;
            break;
        }
        
        //ends loop
        if (kDown & KEY_B) {
            *menu = MENU_ADD_IMAGE_SELECT;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddProcessSelf(u8 *menu, u64 *processID) {
    *processID = 0x000400000ed99000LL;
    *menu = MENU_ADD_NEWS;
}


void menuAddProcessBrowse(u8 *menu, u64 *processID) {
    u32 selected = 0;
    u32 scroll = 0;
    u32 count;
    bool getmedia = true;
    std::vector<u64> titles;
    FS_MediaType media = MEDIATYPE_NAND;
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / B: Back             \x1b[0m");
    
    consoleSelect(&bot);
    printf("\x1b[0;1HSELECT SOURCE:");
    printf("\x1b[1;2HNAND");
    printf("\x1b[2;2HSD");
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //print cursor
        if (getmedia) printf("\x1b[%lu;0H \n>\n ", selected);
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (getmedia)
                selected=(!selected);
            else {
                if (count < 29) {
                if (selected < ( count - 1) ) selected++;
                else selected=0;
                }
                else if (count > 0) {
                    if (selected<14) selected++;
                    else if ( (selected + scroll) < (count - 15) ) scroll++;
                    else if (selected<28) selected++;
                    else { selected = 0; scroll = 0; }
                }
                printTitles(selected, scroll, count, &titles, media);
            }
            
        }
        if (kDown & KEY_UP) {
            if (getmedia)
                selected=(!selected);
            else {
                if (selected>13) selected--;
                else if (scroll>0) scroll--;
                else if (selected>0) selected--;
                else {
                    if (count>28) { selected = 28; scroll = count - 29; }
                    else if (count>0) selected = count - 1;
                }
                printTitles(selected, scroll, count, &titles, media);
            }
        }
        
        //get title or mediatype
        if (kDown & KEY_A) {
            if (getmedia) {
                if (selected==0) media = MEDIATYPE_NAND;
                else media = MEDIATYPE_SD;
                std::vector<u64> newlist;
                AM_GetTitleCount(media, &count);
                u64 *ids = new u64[count];
                AM_GetTitleList(&count, media, count, ids);
                u32 i = 0;
                while (i < count) {
                    newlist.push_back(ids[i]);
                    i++;
                }
                delete[] ids;
                titles.swap(newlist);
                selected = 0;
                printTitles(selected, scroll, count, &titles, media);
                getmedia = false;
            }
            else {
                *processID = titles[selected + scroll];
                *menu = MENU_ADD_NEWS;
                break;
            }
        }
        
        //ends loop
        if (kDown & KEY_B) {
            *menu = MENU_ADD_PROCESS_SELECT;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddProcessNews(u8 *menu, u64 *processID) {
    u32 selected = 0;
    u32 scroll = 0;
    
    u32 total;
    NEWS_GetTotalNotifications(&total);
    
    printNews(selected, scroll, true);
    
    consoleSelect(&top);
    printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / B: Back             \x1b[0m");
    
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        //move cursor
        if (kDown & KEY_DOWN) {
            if (total < 29) {
                if (selected < ( total - 1) ) selected++;
                else selected=0;
            }
            else if (total > 0) {
                if (selected<14) selected++;
                else if ( (selected + scroll) < (total - 15) ) scroll++;
                else if (selected<28) selected++;
                else { selected = 0; scroll = 0; }
            }
            printNews(selected, scroll, true);
            consoleSelect(&top);
            printf("\x1b[28;0H%50c", ' ');
            printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / B: Back             \x1b[0m");
        }
        if (kDown & KEY_UP) {
            if (selected>13) selected--;
            else if (scroll>0) scroll--;
            else if (selected>0) selected--;
            else {
                if (total>28) { selected = 28; scroll = total - 29; }
                else if (total>0) selected = total - 1;
            }
            printNews(selected, scroll, true);
            consoleSelect(&top);
            printf("\x1b[28;0H%50c", ' ');
            printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / B: Back             \x1b[0m");
        }
        
        //select news
        if (kDown & KEY_A) {
            NotificationHeader header;
            NEWS_GetNotificationHeader(selected + scroll, &header);
            *processID = header.processID;
            *menu = MENU_ADD_NEWS;
            break;
        }
        
        //return to previous menu
        if (kDown & KEY_B) {
            *menu = MENU_ADD_PROCESS_SELECT;
            break;
        }
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}


void menuAddNews(u8 *menu, char *title_c, char *message_c, u8 *image, u32 *imgSize, bool *hasImage, u64 *processID) {
    u32 total;
    NEWS_GetTotalNotifications(&total);
    u32 titleSize = strlen(title_c);
    u32 msgSize = strlen(message_c);
    u16* title = (u16*)malloc(0x32 * sizeof(u16));
    u16* message = (u16*)malloc(0x1780 * sizeof(u16));
    ascii2utf(title, title_c);
    ascii2utf(message, message_c);
    Result res = NEWS_AddNotification(title, titleSize, message, msgSize, image, *imgSize, *hasImage);
    free(title);
    free(message);
    NotificationHeader header;
    NEWS_GetNotificationHeader(total, &header);
    header.processID = *processID;
    NEWS_SetNotificationHeader(total, (const NotificationHeader*)&header);
    consoleSelect(&top);
    if (res) printf("\x1b[1;0HFailed! Press any key to continue.");
    else printf("\x1b[1;0HDone! Press any key to continue.");
    waitKey();
    memset(title_c, '\0', 32);
    memset(message_c, '\0', 0x1780);
    *menu = MENU_MAIN;
}