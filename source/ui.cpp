#include <stdio.h>
#include <vector>
#include <string>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>

#include "ui.h"

void printInfo(u8 mode) {
    consoleSelect(&top);
    printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.2%36 \x1b[0m");
    switch (mode) {
        case MODE_NEWS_LIST: { printf("\x1b[28;0H\x1b[47;30mA: Read / Y: Dump / X: Delete / L: Image / B: Back\nR: Unread / SELECT: Dump all / START: Launch app  \x1b[0m"); break; }
        case MODE_FILE_LIST: { printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select / Y: Preview / B: Back\x1b[0m"); break; }
        case MODE_KEYBOARD_TOUCH: { printf("\x1b[28;0H\x1b[47;30mTOUCH: Input / B: Back / L: Backspace / R: Newline\nSELECT: Legacy keyboard / START: Confirm          \x1b[0m"); break; }
        case MODE_KEYBOARD_LEGACY: { printf("\x1b[28;0H\x1b[47;30mD-PAD: Move / A: Input / B: Back / L: Backspace   \nSELECT: Touch keyboard / R: Shift / START: Confirm\x1b[0m"); break; }
        default: { printf("\x1b[29;0H\x1b[47;30mD-PAD: Navigate / A: Select%23\0\x1b[0m"); break; }
    }
}

void printFiles(u32 selected, u32 scroll, u32 count, std::vector<entry> *files, std::string curdir) {
    consoleSelect(&top);
    consoleClear();
    printInfo(MODE_FILE_LIST);
    consoleSelect(&bot);
    bool isRoot = (curdir=="/");
    printf("\x1b[0;0H%.40s", curdir.c_str());
    u32 i = 0;
    while (i < count) {
        if (i > 27) break;
        if ( (*files)[i+scroll].isDir ) printf("\x1b[%lu;0H\x1b[33m  %.38s\x1b[0m", 2 + i, (*files)[i+scroll].name.c_str());
        else printf("\x1b[%lu;0H  %.38s", 2 + i, (*files)[i+scroll].name.c_str());
        i++;
    }
    if (isRoot) printf("\x1b[1;0H\x1b[35m  [root]\x1b[0m");
    else printf("\x1b[1;0H\x1b[37m  ..\x1b[0m");
    printf("\x1b[%lu;0H>", 1 + selected);
}

void printTitles(u32 selected, u32 scroll, u32 count, std::vector<u64> *titles, FS_MediaType media) {
    consoleSelect(&bot);
    if (media==MEDIATYPE_SD) printf("\x1b[0;0H/SD");
    else printf("\x1b[0;0H/NAND");
    u32 i = 0;
    while (i < count) {
        if (i > 28) break;
        printf("\x1b[%lu;0H  %#llx", 1 + i, (*titles)[i+scroll]);
        i++;
    }
    printf("\x1b[%lu;0H>", 1 + selected);
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
    gfxInitDefault();
    consoleInit(GFX_TOP, &top);
    consoleInit(GFX_BOTTOM, &bot);
}

void gfxEndFrame(void) {
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}
