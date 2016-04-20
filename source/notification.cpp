#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "notification.h"
#include "utils.h"

/*
void addNews(const char *title_c, const char *text_c, u8 imgOption, NewsList* list) {
    u32 titleSize = strlen(title_c);
    u32 textSize = strlen(text_c);
    u32 imgSize = 0;
    u16* title = (u16*)malloc(titleSize*sizeof(u16));
    u16* text = (u16*)malloc(textSize*sizeof(u16));
    u8* image = (u8*)calloc(0x20000, 1);
    ascii2utf(title,(char*)title_c);
    ascii2utf(text,(char*)text_c);
    if (imgOption==0) {
        Handle imgHandle;
        FS_Archive sdmcArchive = (FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
        char filePath[] = "/NotifyMii/default.jpg";
        FS_Path imgPath = fsMakePath(PATH_ASCII, filePath);
        Result res = FSUSER_OpenFileDirectly( &imgHandle, sdmcArchive, imgPath, FS_OPEN_READ, 0x00000000);
        if (!res) {
            u64 size;
            FSFILE_GetSize(imgHandle, &size);
            FSFILE_Read(imgHandle, &imgSize, 0, image, size);
            FSFILE_Close(imgHandle);
        }
        svcCloseHandle(imgHandle);
    }
    if (imgOption==1) {
        u32 id = selectNews(list);
		NEWS_GetNotificationImage(id, image, &imgSize);
    }
    if (imgOption==2) {
        image = NULL;
    }
    Result res = NEWS_AddNotification(title, titleSize, text, textSize, image, imgSize, true);
    consoleSelect(&top);
    if (res) printf("\x1b[1;0HFailed! Press any key to continue.");
    else {
        printf("\x1b[1;0HDone! Press any key to continue.");
    }
    waitKey();
}
*/

void printNews(u32 selected, u32 scroll, bool info) {
    NotificationHeader header;
    if (info) {
        consoleSelect(&top);
        consoleClear();
        NEWS_GetNotificationHeader(selected, &header);
        printf("\x1b[1;0HProcess ID: %#0llx", header.processID);
        u64 time = (header.time / 1000) % 86400;
        u8 hours = time / 3600;
        u8 minutes = (time % 3600) / 60;
        u8 seconds = time % 60;
        printf("\x1b[2;0HTime: 00:00:00\x1b[2;6H%2u:%2u:%2u", hours, minutes, seconds);
        printf("\x1b[3;0HFlags:\x1b[4;0H");
        if (header.unread) printf("\x1b[33m  Unread\n\x1b[0m");
        if (header.isSpotPass) printf("\x1b[32m  SpotPass\n\x1b[0m");
        if (header.enableJPEG) printf("\x1b[35m  Has image\n\x1b[0m");
        if (header.isOptedOut) printf("\x1b[31m  Opted out\n\x1b[0m");
        printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.1                                    \x1b[0m");
        printf("\x1b[28;0H\x1b[47;30mA: Read / Y: Dump / X: Delete / B: Back           \x1b[0m");
        printf("\x1b[29;0H\x1b[47;30mSELECT: Dump all / START: Launch app              \x1b[0m");
    }
    consoleSelect(&bot);
    consoleClear();
    printf("\x1b[0;0HNOTIFICATION LIST");
    printf("\x1b[%lu;0H>", 1 + selected);
    
    u32 total;
    NEWS_GetTotalNotifications(&total);
    
	u32 i = 0;
	while (i < total) {
		if (i > 28) break;
        NEWS_GetNotificationHeader(i + scroll, &header);
        char title[32];
        utf2ascii(title, header.title);
		if (header.unread) printf("\x1b[%lu;2H\x1b[33m%s [!]\x1b[0m", 1 + i, title);
        else printf("\x1b[%lu;2H%s", 1 + i, title);
		i++;
	}
}


void readNews(u32 id) {
    consoleSelect(&top);
    consoleClear();
    NotificationHeader header;
    NEWS_GetNotificationHeader(id, &header);
    header.unread=false;
    NEWS_SetNotificationHeader(id, (const NotificationHeader*)&header);
	u16 tmp[0x1780];
	NEWS_GetNotificationMessage(id, tmp);
	char message[0x1780];
	utf2ascii(message, tmp);
	printf("\x1b[1;0H%s", message);
    printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.1                                    \x1b[0m");
    printf("\x1b[28;0H\x1b[47;30mA: Read / Y: Dump / X: Delete / B: Back           \x1b[0m");
    printf("\x1b[29;0H\x1b[47;30mSELECT: Dump all / START: Launch app              \x1b[0m");
    consoleSelect(&bot);
}


void dumpNews (u32 id) {
    //generating header
    NotificationHeader header;
    NEWS_GetNotificationHeader(id, &header);
    char title[32];
    utf2ascii(title, header.title);
    
    //generating filenames
	u64 time = (osGetTime() / 1000) % 86400;
	u8 hours = time / 3600;
	u8 minutes = (time % 3600) / 60;
	u8 seconds = time % 60;
	char filename[64];
	char filename2[64];
	sprintf(filename, "/NotifyMii/%u%u%u_%s.txt", hours, minutes, seconds, title);
	sprintf(filename2,"/NotifyMii/%u%u%u_%s.jpg", hours, minutes, seconds, title);
	
	//getting message
	u16 tmp[0x1780];
	NEWS_GetNotificationMessage(id, tmp);
	char message[0x1780 * 2];
	utf2ascii(message, tmp);
	
	//writing message file
	u32 bytes;
	Handle fileHandle;
	FS_Archive sdmcArchive=(FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
	FS_Path filePath=fsMakePath(PATH_ASCII, filename);
	FSUSER_OpenFileDirectly( &fileHandle, sdmcArchive, filePath, FS_OPEN_WRITE | FS_OPEN_CREATE, 0x00000000);
	FSFILE_Write(fileHandle, &bytes, 0, message, strlen(message), 0);
	FSFILE_Close(fileHandle);
	svcCloseHandle(fileHandle);
	
	//writing image file if exists
	if (header.enableJPEG) {
		u32 size;
		Handle fileHandle2;
		u8* buffer = (u8*)malloc(0x20000);
		Result ret = NEWS_GetNotificationImage(id, buffer, &size);
        if (!ret) {
            FS_Path filePath2=fsMakePath(PATH_ASCII, filename2);
            FSUSER_OpenFileDirectly( &fileHandle2, sdmcArchive, filePath2, FS_OPEN_WRITE | FS_OPEN_CREATE, 0x00000000);
            FSFILE_Write(fileHandle2, &bytes, 0, buffer, size, 0);
            FSFILE_Close(fileHandle2);
        }
		svcCloseHandle(fileHandle2);
		free(buffer);
	}
}


void deleteNews(u32 id) {
    /*
    u32 total;
    NEWS_GetTotalNotifications(&total);
    u32 i = id;
    while (i < (total-1) ) {
        NotificationHeader header;
        NEWS_GetNotificationHeader(i + 1, &header);
        NEWS_SetNotificationHeader(i, (const NotificationHeader*)&header);
        i++;
    }
    */
    NotificationHeader header = { 0 };
    NEWS_SetNotificationHeader(id, (const NotificationHeader*)&header);
}