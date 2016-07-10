#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "utils.h"
#include "notification.h"
#include "ui.h"

void readNews(u32 id) {
    consoleSelect(&top);
    consoleClear();
    NotificationHeader header;
    NEWS_GetNotificationHeader(id, &header);
    header.unread = false;
    NEWS_SetNotificationHeader(id, (const NotificationHeader*)&header);
    u16 tmp[0x1780];
    NEWS_GetNotificationMessage(id, tmp, NULL);
    char message[0x1780];
    utf2ascii(message, tmp);
    printf("\x1b[1;0H%s", message);
    printInfo(MODE_NEWS_LIST);
}

// credits: Rinnegatamante
void dumpNews (u32 id) {
    // getting header and title
    NotificationHeader header;
    NEWS_GetNotificationHeader(id, &header);
    char title[32];
    utf2ascii(title, header.title);

    // generating filenames
    u64 time = (osGetTime() / 1000) % 86400;
    u8 hours = time / 3600;
    u8 minutes = (time % 3600) / 60;
    u8 seconds = time % 60;
    std::string titlef = validateFileName(std::string(title));
    char fname_msg[64];
    char fname_img[64];
    sprintf(fname_msg, "/NotifyMii/%02u%02u%02u_%s.txt", hours, minutes, seconds, titlef.c_str());
    sprintf(fname_img, "/NotifyMii/%02u%02u%02u_%s.jpg", hours, minutes, seconds, titlef.c_str());

    // getting message
    u16 tmp[0x1780];
    NEWS_GetNotificationMessage(id, tmp, NULL);
    char message[0x1780 * 2];
    utf2ascii(message, tmp);

    // writing message to file
    u32 bytes;
    Handle fileHandle;
    FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}, (FS_Path)fsMakePath(PATH_ASCII, fname_msg), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);
    FSFILE_Write(fileHandle, &bytes, 0, message, strlen(message), 0);
    FSFILE_Close(fileHandle);
    svcCloseHandle(fileHandle);

    // writing image to file, if an image exists
    if (header.enableJPEG) {
        u32 size;
        Handle fileHandle2;
        u8* buffer = (u8*)malloc(0xC800);
        Result ret = NEWS_GetNotificationImage(id, buffer, &size);
        if (!ret) {
            FSUSER_OpenFileDirectly(&fileHandle2, ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}, (FS_Path)fsMakePath(PATH_ASCII, fname_img), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);
            FSFILE_Write(fileHandle2, &bytes, 0, buffer, (size > 0xC800) ? 0xC800 : size, 0);
            FSFILE_Close(fileHandle2);
        }
        svcCloseHandle(fileHandle2);
        free(buffer);
    }
}

void deleteNews(u32 id) {
    u32 total;
    NEWS_GetTotalNotifications(&total);
    u32 i = id;
    while ((i < (total-1)) && (i < 99)) {
        NotificationHeader header;
        u16 tmp[0x1780];
        u32 msgSize;
        u32 imgSize;
        u8* img = (u8*)malloc(0xC800);
        NEWS_GetNotificationHeader(i + 1, &header);
        NEWS_GetNotificationMessage(i + 1, tmp, &msgSize);
        NEWS_GetNotificationImage(i + 1, img, &imgSize);
        NEWS_SetNotificationHeader(i, (const NotificationHeader*)&header);
        NEWS_SetNotificationMessage(i, (const u16*)tmp, msgSize);
        NEWS_SetNotificationImage(i, (const u8*)img, (imgSize > 0xC800) ? 0xC800 : imgSize);
        free(img);
        i++;
    }
    // credits: Rinnegatamante
    NotificationHeader header = { 0 };
    NEWS_SetNotificationHeader(total - 1, (const NotificationHeader*)&header);
}

void clearNews() {
    u32 total;
    u32 deleted = 0;
	NotificationHeader header = { 0 };
    NEWS_GetTotalNotifications(&total);
    u32 i = 0;
    printInfo(MODE_NEWS_LIST);
	while (i < total) {
		Result ret = NEWS_SetNotificationHeader(i, (const NotificationHeader*)&header);
        if (!ret) deleted++;
        i++;
        printf("\x1b[27;0H%lu notifications deleted.", deleted);
        gfxEndFrame();
	}
}
