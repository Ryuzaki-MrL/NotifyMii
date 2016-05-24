#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sf2d.h>
#include <sfil.h>

#include "utils.h"
#include "ui.h"

void printInfo(u8 mode, bool clear) {
    consoleSelect(&top);
    if (clear) consoleClear();
    printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.2                                    \x1b[0m");
    switch (mode) {
        case MODE_NEWS_LIST: { printf("\x1b[28;0H\x1b[47;30mA: Read / Y: Dump / X: Delete / R: Image / B: Back\x1b[29;0HL: Mark / SELECT: Extra / START: Launch software  \x1b[0m"); break; }
        case MODE_FILE_LIST: { printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m"); break; }
        case MODE_KEYBOARD_TOUCH: { printf("\x1b[29;0H\x1b[47;30mTOUCH: Input / START: Confirm / B: Cancel         \x1b[0m"); break; }
        case MODE_KEYBOARD_LEGACY: { printf("\x1b[28;0H\x1b[47;30mD-PAD: Move / A: Input / B: Cancel / L: Backspace \x1b[29;0HR: Shift / START: Confirm                         \x1b[0m"); break; }
        case MODE_NONE: break;
        default: { printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select                       \x1b[0m"); break; }
    }
}

void printFiles(u32 selected, u32 scroll, u32 count, std::vector<entry> *files, std::string curdir) {
    printInfo(MODE_FILE_LIST);
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

void printTitles(u32 selected, u32 scroll, u32 count, std::vector<title_entry> *titles, FS_MediaType media) {
    consoleSelect(&bot);
    consoleClear();
    if (media==MEDIATYPE_SD) printf("\x1b[0;0H/SD");
    else printf("\x1b[0;0H/NAND");
    u32 i = 0;
    while (i < count) {
        if (i > 28) break;
        printf("\x1b[%lu;2H%.22s\x1b[%lu;25H%#llx", 1 + i, (*titles)[i+scroll].name.c_str(), 1 + i, (*titles)[i+scroll].id);
        i++;
    }
    printf("\x1b[%lu;0H>", 1 + selected);
}

void printNews(u32 selected, u32 scroll, bool info) {
    NotificationHeader header;
    if (info) {
        printInfo(MODE_NEWS_LIST, true);
        NEWS_GetNotificationHeader(selected+scroll, &header);
        printf("\x1b[1;0HProcess ID: %#0llx", header.processID);
        // printf("\x1b[2;0HJump Parameter: %#0llx", header.jumpParam);
        u64 time = (header.time / 1000) % 86400;
        u8 hours = time / 3600;
        u8 minutes = (time % 3600) / 60;
        u8 seconds = time % 60;
        printf("\x1b[2;0HTime: %02u:%02u:%02u", hours, minutes, seconds);
        printf("\x1b[3;0HFlags:\x1b[4;0H");
        if (!header.dataSet) printf("\x1b[37m  Invalid\n\x1b[0m");
        if (header.unread) printf("\x1b[33m  Unread\n\x1b[0m");
        if (header.isSpotPass) printf("\x1b[34;1m  SpotPass\n\x1b[0m");
        if (header.enableJPEG) printf("\x1b[35m  Has image\n\x1b[0m");
        if (header.isOptedOut) printf("\x1b[31m  Opted out\n\x1b[0m");
    }
    consoleSelect(&bot);
    consoleClear();
    printf("\x1b[0;0HNOTIFICATION LIST");
    u32 total;
    NEWS_GetTotalNotifications(&total);
    u32 i = 0;
    while (i < total) {
        if (i > 28) break;
        NEWS_GetNotificationHeader(i + scroll, &header);
        char title[32];
        utf2ascii(title, header.title);
        if (header.unread) printf("\x1b[%lu;0H\x1b[33m  %.44s [!]\x1b[0m", 1 + i, title);
        else printf("\x1b[%lu;0H  %.48s", 1 + i, title);
        i++;
    }
    printf("\x1b[%lu;0H>", 1 + selected);
    consoleSelect(&top);
}

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
    if (aptMainLoop()) {
        gfxInitDefault();
        consoleInit(GFX_TOP, &top);
        consoleInit(GFX_BOTTOM, &bot);
    }
}

bool promptConfirm(std::string strg) {
    consoleSelect(&top);
    consoleClear();
    printf("\x1b[14;%uH%s", (25 - (strg.size() / 2)), strg.c_str());
    printf("\x1b[16;14HA: Confirm / B: Cancel");
    u32 kDown = 0;
    while ( aptMainLoop() ) {
        hidScanInput();
        kDown = hidKeysDown();
        if (kDown) break;
        gfxEndFrame();
    }
    if (kDown & KEY_A) return true;
    else return false;
}

void promptAlert(std::string strg) {
    consoleSelect(&top);
    consoleClear();
    printf("\x1b[14;%uH%s", (25 - (strg.size() / 2)), strg.c_str());
    while ( aptMainLoop() ) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown) break;
        gfxEndFrame();
    }
}

void gfxEndFrame() {
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}
