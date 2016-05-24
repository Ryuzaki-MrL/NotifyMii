#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack>
#include <vector>
#include <3ds.h>

#include "utils.h"
#include "notification.h"
#include "menu.h"
#include "ui.h"
#include "keyboard.h"

u8 selectionMenu(std::string text, u8 entries) {
    u8 selected = 0;
    printInfo(MODE_SELECTION, true);
    consoleSelect(&bot);
    consoleClear();
    printf("\x1b[0;1H%s", text.c_str());
    while ( aptMainLoop() ) {
        printf("\x1b[%u;0H \n>\n ", selected);
        hidScanInput();
        u32 kDown = hidKeysDown();
        if ( (kDown & KEY_DOWN) && (selected < entries-1) ) selected++;
        if ( (kDown & KEY_UP) && (selected > 0) ) selected--;
        if (kDown & KEY_A) break;
        if (kDown & KEY_B) return 0xFF;
        gfxEndFrame();
    }
    return selected;
}

u8 getNotificationID() { // TODO: finish preview support; print correct info
    u32 selected = 0;
    u32 scroll = 0;
    u32 total;
    NEWS_GetTotalNotifications(&total);
    printInfo(MODE_FILE_LIST);
    printNews(selected, scroll, true);
    while (aptMainLoop()) {
        if (total==0) {
            consoleSelect(&bot);
            consoleClear();
            printf("\x1b[0;0HYou don't have any notification!\nPress any key to continue.");
            waitKey();
            return 0xFF;
        }
        hidScanInput();
        u32 kDown = hidKeysDown();
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
        if (kDown & KEY_A) break;
        if (kDown & KEY_B) return 0xFF;
        if (kDown & KEY_Y) {
            u32 size;
            NotificationHeader header;
            NEWS_GetNotificationHeader(selected + scroll, &header);
            if (header.enableJPEG) {
                u8* buffer = (u8*)malloc(0x10000);
                NEWS_GetNotificationImage(selected + scroll, buffer, &size);
                drawImage(buffer, size);
                free(buffer);
                printNews(selected, scroll, true);
            }
        }
        gfxEndFrame();
    }
    return selected + scroll;
}

u64 getTitleID() {
    u32 selected = 0;
    u32 scroll = 0;
    u32 count;
    FS_MediaType media;
    std::vector<title_entry> titlelist;
    printInfo(MODE_SELECTION);
    bool getmedia = true;
    while ( aptMainLoop() )
    {
        if (getmedia) {
            media = (FS_MediaType)selectionMenu("SELECT SOURCE:\n  NAND\n  SD", 2);
            if (media==0xFF) return 0;
            AM_GetTitleCount(media, &count);
            if (count==0) return 0;
            u64 *ids = new u64[count];
            AM_GetTitleList(&count, media, count, ids);
            titlelist.clear();
            u32 i = 0;
            while ( (i < count) && aptMainLoop() ) {
                Handle smdhHandle;
                char shortDesc[0x40];
                u32 archivePathData[] = {(u32)(ids[i] & 0xFFFFFFFF), (u32)(ids[i] >> 32), media, 0x00000000};
                const u32 filePathData[] = {0x00000000, 0x00000000, 0x00000002, 0x6E6F6369, 0x00000000};
                Result ret = FSUSER_OpenFileDirectly(&smdhHandle, ARCHIVE_SAVEDATA_AND_CONTENT, (FS_Path){PATH_BINARY, 0x10, (u8*)archivePathData}, (FS_Path){PATH_BINARY, 0x14, (u8*)filePathData}, FS_OPEN_READ, 0);
                if (!ret) {
                    SMDH smdh;
                    u32 bytesRead;
                    FSFILE_Read(smdhHandle, &bytesRead, 0x0, &smdh, sizeof(SMDH));
                    FSFILE_Close(smdhHandle);
                    u8 lang = CFG_LANGUAGE_EN;
                    CFGU_GetSystemLanguage(&lang);
                    utf2ascii(shortDesc, smdh.titles[lang].shortDescription);
                }
                else memset(shortDesc, 0, 0x40);
                titlelist.push_back({ids[i], std::string(shortDesc)});
                i++;
                printf("\x1b[29;0HLoaded %lu titles", i);
                gfxEndFrame();
            }
            delete[] ids;
            selected = 0;
            scroll = 0;
            printTitles(selected, scroll, count, &titlelist, media);
            getmedia = false;
        }
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_DOWN) {
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
            printTitles(selected, scroll, count, &titlelist, media);
        }
        if (kDown & KEY_UP) {
            if (selected>13) selected--;
            else if (scroll>0) scroll--;
            else if (selected>0) selected--;
            else {
                if (count>28) { selected = 28; scroll = count - 29; }
                else if (count>0) selected = count - 1;
            }
            printTitles(selected, scroll, count, &titlelist, media);
        }
        if (kDown & KEY_A) break;
        if (kDown & KEY_B) getmedia = true;
        gfxEndFrame();
    }
    return titlelist[selected + scroll].id;
}

std::string getFileName(std::string filter) {
    u32 selected = 0;
    u32 scroll = 0;
    std::string curdir = "/";
    std::stack<std::string> innerpath;
    std::vector<entry> filelist = getFileList(curdir, filter);
    u32 count = filelist.size();
    printFiles(selected, scroll, count, &filelist, curdir);
    while ( aptMainLoop() ) {
        hidScanInput();
        u32 kDown = hidKeysDown();
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
        if (kDown & KEY_A) {
            if (selected > 0) {
                if (filelist[selected+scroll-1].isDir) {
                    innerpath.push(curdir);
                    curdir = curdir + filelist[selected+scroll-1].name + "/";
                    std::vector<entry> newlist = getFileList(curdir, filter);
                    filelist.swap(newlist);
                    selected = 0; scroll = 0; count = filelist.size();
                    printFiles(selected, scroll, count, &filelist, curdir);
                }
                else break;
            }
        }
        if (kDown & KEY_B) {
            if (curdir=="/") return "";
            else if (!innerpath.empty()) {
                curdir = innerpath.top();
                innerpath.pop();
                std::vector<entry> newlist = getFileList(curdir, filter);
                filelist.swap(newlist);
                selected = 0; scroll = 0; count = filelist.size();
                printFiles(selected, scroll, count, &filelist, curdir);
            }
        }
        if (kDown & KEY_Y) {
            consoleSelect(&top);
            Handle fileHandle;
            std::string path = curdir + filelist[selected+scroll-1].name;
            Result res = FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}, (FS_Path)fsMakePath(PATH_ASCII, path.c_str()), FS_OPEN_READ, 0);
            if (res) {
                printf("\x1b[1;0HCouldn't open file.\nPress any key to continue.");
                waitKey();
            }
            else {
                if (filter=="jpg") {
                    u64 size;
                    FSFILE_GetSize(fileHandle, &size);
                    if ( (size > 0xC800) ) {
                        printf("\x1b[1;0HImage file is too large.\nPress any key to continue.");
                        waitKey();
                    }
                    else if (size > 0) {
                        u8* buffer = (u8*)malloc(0xC800);
                        u32 bufferSize;
                        FSFILE_Read(fileHandle, &bufferSize, 0, buffer, size);
                        FSFILE_Close(fileHandle);
                        drawImage(buffer, bufferSize);
                        free(buffer);
                        printFiles(selected, scroll, count, &filelist, curdir);
                    }
                    else {
                        printf("\x1b[1;0HInvalid file.\nPress any key to continue.");
                        waitKey();
                    }
                }
                else {
                    u64 size;
                    FSFILE_GetSize(fileHandle, &size);
                    if (size > 0x1780) {
                        printf("\x1b[1;0HText file is too large.\nPress any key to continue.");
                        waitKey();
                    }
                    else {
                        char buffer[0x1780] = { 0 };
                        FSFILE_Read(fileHandle, NULL, 0, buffer, size);
                        FSFILE_Close(fileHandle);
                        consoleClear();
                        printf("\x1b[1;0H%s", buffer);
                        printInfo(MODE_FILE_LIST);
                    }
                }
            }
            svcCloseHandle(fileHandle);
        }
        gfxEndFrame();
    }
    return curdir + filelist[selected+scroll-1].name;
}

void menuMain(u8 *menu) {
    printInfo(MODE_SELECTION);
    *menu = selectionMenu("MAIN MENU\n  Create Notification\n  Notification List\n  Exit", 3) + 1;
}

void menuNewsList(u8 *menu) { // TODO: use selection menus for actions
    u32 selected = 0;
    u32 scroll = 0;
    u32 total;
    NEWS_GetTotalNotifications(&total);

    printNews(selected, scroll, true);

    while ( aptMainLoop() )
    {
        // return if there are no notifications
        if (total==0) {
            consoleSelect(&bot);
            consoleClear();
            printf("\x1b[0;0HYou don't have any notification!\nPress any key to continue.");
            waitKey();
            *menu = MENU_MAIN;
            break;
        }

        hidScanInput();
        u32 kDown = hidKeysDown();

        // move cursor
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

        // read news
        if (kDown & KEY_A) {
            readNews(selected + scroll);
            printNews(selected, scroll);
        }

        // view image
        if (kDown & KEY_R) {
            u32 size;
            NotificationHeader header;
            NEWS_GetNotificationHeader(selected + scroll, &header);
            if (header.enableJPEG) {
                u8* buffer = (u8*)malloc(0x10000);
                NEWS_GetNotificationImage(selected + scroll, buffer, &size);
                drawImage(buffer, size);
                free(buffer);
                printNews(selected, scroll, true);
            }
        }

        // dump news
        if (kDown & KEY_Y) {
            dumpNews(selected + scroll);
            printf("\x1b[27;0HDumped 1 notification to 'SD:/NotifyMii'.");
        }

        // extra options
        if (kDown & KEY_SELECT) {
            u8 option = selectionMenu("EXTRA OPTIONS\n  Dump all\n  Delete all\n  Cancel", 3);
            consoleSelect(&top);
            if (option==0) {
                u32 i = 0;
                while (i < total) {
                    dumpNews(i);
                    i++;
                    printf("\x1b[27;0HDumped %lu notifications to 'SD:/NotifyMii'.", i);
                    gfxEndFrame();
                }
                // printf("\x1b[27;0HDumped %lu notifications to 'SD:/NotifyMii'. Done!", i);
            }
            else if (option==1) clearNews();
            printNews(selected, scroll, true);
        }

        // delete news
        if (kDown & KEY_X) {
            deleteNews(selected + scroll);
            NEWS_GetTotalNotifications(&total);
            if (scroll>0) scroll--;
            else if (selected>0) selected--;
            printNews(selected, scroll, true);
        }
        
        // mark news
        if (kDown & KEY_L) printf("\x1b[27;0HNot implemented yet.");

        // launch software
        if (kDown & KEY_START) {
            NotificationHeader header;
            NEWS_GetNotificationHeader(selected + scroll, &header);
            u8 media = MEDIATYPE_SD;
            while (header.processID > 0) {
                u32 count;
                AM_GetTitleCount((FS_MediaType)media, &count);
                u64 *ids = new u64[count];
                AM_GetTitleList(&count, (FS_MediaType)media, count, ids);
                u32 i = 0;
                while ((ids[i] != header.processID) && (i < count)) i++;
                delete[] ids;
                if (i==count) {
                    if (media==MEDIATYPE_SD) media = MEDIATYPE_NAND;
                    else {
                        printf("\x1b[27;0HTitle not found.");
                        break;
                    }
                    i = 0;
                }
                else {
                    u8 buf0[0x300];
                    u8 buf1[0x20];
                    memset(buf0, 0, 0x300);
                    memset(buf1, 0, 0x20);
                    aptOpenSession();
                    APT_PrepareToDoAppJump(0, header.processID, media);
                    APT_DoAppJump(0x300, 0x20, buf0, buf1);
                    aptCloseSession();
                    break;
                }
            }
        }

        // return to main menu
        if (kDown & KEY_B) {
            *menu = MENU_MAIN;
            break;
        }
        
        gfxEndFrame();
    }
}

void menuNewsAddTitle(u8 *menu, char *title) {
    printInfo(MODE_SELECTION);
    u8 option = selectionMenu("SELECT INPUT METHOD:\n  Touch keyboard\n  Legacy keyboard\n  Cancel", 3);
    consoleSelect(&top);
    if (option < 2) {
        printf("\x1b[1;0HNOTIFICATION TITLE:");
        std::string buffer;
        // if (option==0) buffer = getKeyboardInput(32, std::string(title));
        // else buffer = getKeyboardInputLegacy(32, std::string(title));
        if (option==0) buffer = getKeyboardInput(32);
        else buffer = getKeyboardInputLegacy(32);
        memcpy(title, buffer.c_str(), 32);
        if (buffer == "") return;
        else *menu = MENU_NEWS_ADD_MESSAGE;
    }
    else *menu = MENU_MAIN;
}

void menuNewsAddMessage(u8 *menu, char *message) {
    printInfo(MODE_SELECTION);
    u8 option = selectionMenu("SELECT MESSAGE SOURCE:\n  Type a message (Touch keyboard)\n  Type a message (Legacy keyboard)\n  Use a text file\n  Use a notification\n  Cancel", 5);
    consoleSelect(&top);
    switch (option) {
        case 0: case 1: {
            printf("\x1b[1;0HNOTIFICATION MESSAGE:");
            std::string buffer;
            // if (option==0) buffer = getKeyboardInput(0x1780, std::string(message));
            // else buffer = getKeyboardInputLegacy(0x1780, std::string(message));
            if (option==0) buffer = getKeyboardInput(0x1780);
            else buffer = getKeyboardInputLegacy(0x1780);
            memcpy(message, buffer.c_str(), 0x1780);
            if (buffer == "") break;
            else *menu = MENU_NEWS_ADD_IMAGE;
            break;
        }
        case 2: {
            std::string filepath = getFileName("txt");
            if (filepath=="") break;
            u32 bytes;
            Handle fileHandle;
            Result res = FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}, (FS_Path)fsMakePath(PATH_ASCII, filepath.c_str()), FS_OPEN_READ, 0);
            if (res) {
                printf("\x1b[1;0HCouldn't open file.\nPress any key to continue.");
                waitKey();
            }
            else {
                u64 size;
                FSFILE_GetSize(fileHandle, &size);
                if (size > 0x1780) {
                    printf("\x1b[1;0HText file is too large.\nPress any key to continue.");
                    waitKey();
                }
                else {
                    memset(message, '\0', 0x1780);
                    Result res = FSFILE_Read(fileHandle, &bytes, 0, message, size);
                    FSFILE_Close(fileHandle);
                    if (!res) *menu = MENU_NEWS_ADD_IMAGE;
                    else {
                        printf("\x1b[1;0HError reading file.\nPress any key to continue.");
                        waitKey();
                    }
                }
            }
            svcCloseHandle(fileHandle);
            break;
        }
        case 3: {
            u8 newsid = getNotificationID();
            if (newsid==0xFF) break;
            u16 buffer[0x1780];
            Result res = NEWS_GetNotificationMessage(newsid, buffer, NULL);
            utf2ascii(message, buffer);
            if (!res) *menu = MENU_NEWS_ADD_IMAGE;
            else {
                printf("\x1b[1;0HFailed to get notification's message.\nPress any key to continue.");
                waitKey();
            }
            break;
        }
        default: {
            *menu = MENU_NEWS_ADD_TITLE;
            break;
        }
    }
}

void menuNewsAddImage(u8 *menu, u8 *image, u32 *imgSize) {
    printInfo(MODE_SELECTION);
    u8 option = selectionMenu("SELECT IMAGE SOURCE:\n  Use default.jpg\n  Select from SD card\n  Select from notification\n  Nintendo 3DS Camera\n  No image\n  Cancel", 6);
    switch (option) {
        case 0: case 1: {
            std::string filepath = "/NotifyMii/default.jpg";
            if (option==1) filepath = getFileName("jpg");
            if (filepath=="") break;
            consoleSelect(&top);
            Handle imgHandle;
            Result res = FSUSER_OpenFileDirectly(&imgHandle, ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}, (FS_Path)fsMakePath(PATH_ASCII, filepath.c_str()), FS_OPEN_READ, 0);
            if (res) {
                printf("\x1b[1;0HCouldn't open 'default.jpg'.\nPress any key to continue.");
                waitKey();
            }
            else {
                u64 size;
                FSFILE_GetSize(imgHandle, &size);
                if (size > 0xC800) {
                    printf("\x1b[1;0HImage file is too large.\nPress any key to continue.");
                    waitKey();
                }
                else {
                    Result res = FSFILE_Read(imgHandle, imgSize, 0, image, size);
                    FSFILE_Close(imgHandle);
                    if (!res) *menu = MENU_NEWS_ADD_PROCESS;
                    else {
                        printf("\x1b[1;0HError reading file.\nPress any key to continue.");
                        waitKey();
                    }
                }
            }
            svcCloseHandle(imgHandle);
            break;
        }
        case 2: {
            u8 newsid = getNotificationID();
            if (newsid==0xFF) break;
            consoleSelect(&top);
            NotificationHeader header;
            NEWS_GetNotificationHeader(newsid, &header);
            if (header.enableJPEG) {
                Result res = NEWS_GetNotificationImage(newsid, image, imgSize);
                if (!res) *menu = MENU_NEWS_ADD_PROCESS;
                else {
                    printf("\x1b[1;0HFailed to get notification's image.\nPress any key to continue.");
                    waitKey();
                }
            }
            else {
                printf("\x1b[1;0HNotification has no image.\nPress any key to continue.");
                waitKey();
            }
            break;
        }
        case 3: {
            printf("\x1b[1;0HNot implemented yet.\nPress any key to continue.");
            waitKey();
            break;
        }
        case 4: {
            *imgSize = 0;
            *menu = MENU_NEWS_ADD_PROCESS;
            break;
        }
        default: {
            *menu = MENU_NEWS_ADD_MESSAGE;
            break;
        }
    }
}

void menuNewsAddProcess(u8 *menu, u64 *processID) {
    printInfo(MODE_SELECTION);
    u8 option = selectionMenu("SELECT PROCESS ID SOURCE:\n  NotifyMii\n  Select from installed title\n  Select from notification\n  Default\n  Cancel", 5);
    consoleSelect(&top);
    switch (option) {
        case 0: {
            *processID = 0x000400000ed99000LL;
            *menu = MENU_NEWS_ADD;
            break;
        }
        case 1: {
            u64 tid = getTitleID();
            if (tid==0) break;
            *processID = tid;
            *menu = MENU_NEWS_ADD;
            break;
        }
        case 2: {
            u8 newsid = getNotificationID();
            if (newsid==0xFF) break;
            consoleSelect(&top);
            NotificationHeader header;
            Result res = NEWS_GetNotificationHeader(newsid, &header);
            *menu = MENU_NEWS_ADD;
            if (!res) *processID = header.processID;
            else {
                printf("\x1b[1;0HFailed to get notification header.\nPress any key to continue.");
                waitKey();
                *menu = MENU_NEWS_ADD_PROCESS;
            }
            break;
        }
        case 3: {
            *processID = 0;
            *menu = MENU_NEWS_ADD;
            break;
        }
        default: {
            *menu = MENU_NEWS_ADD_IMAGE;
            break;
        }
    }
}

void menuAddNews(u8 *menu, char *title_c, char *message_c, u8 *image, u32 imgSize, u64 processID, news_flags flags) {
    u32 total;
    NEWS_GetTotalNotifications(&total);
    u32 titleSize = strlen(title_c);
    u32 msgSize = strlen(message_c);
    u16* title = (u16*)malloc(0x32 * sizeof(u16));
    u16* message = (u16*)malloc(0x1780 * sizeof(u16));
    ascii2utf(title, title_c);
    ascii2utf(message, message_c);
    bool hasImage = true;
    if (imgSize==0) {
        hasImage = false;
        memset(image, 0, 0xC800);
        imgSize = 0xC800;
    }
    Result res = NEWS_AddNotification(title, titleSize, message, msgSize, image, imgSize, hasImage);
    free(title);
    free(message);
    consoleSelect(&top);
    if (res) printf("\x1b[1;0HFailed! Press any key to continue.");
    else {
        NotificationHeader header;
        NEWS_GetNotificationHeader(total, &header);
        header.processID = processID;
        header.unread = flags.unread;
        header.isSpotPass = flags.isSpotPass;
        header.isOptedOut = flags.isOptedOut;
        NEWS_SetNotificationHeader(total, (const NotificationHeader*)&header);
        printf("\x1b[1;0HDone! Press any key to continue.");
    }
    waitKey();
    memset(title_c, '\0', 32);
    memset(message_c, '\0', 0x1780);
    memset(image, 0, 0xC800);
    *menu = MENU_MAIN;
}