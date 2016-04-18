#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "notification.h"
#include "utils.h"

NewsList* createVoice(u32 id) {
	NewsList* voice = (NewsList*)malloc(sizeof(NewsList));
	NotificationHeader header;
	NEWS_GetNotificationHeader(id, &header);
	utf2ascii(voice->title, header.title);
	voice->next = NULL;
	voice->id = id;
	voice->hasImage = header.enableJPEG;
    voice->unread = header.unread;
    voice->isSpotPass = header.isSpotPass;
	return voice;
}

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
    /*
    if (imgOption==2) {
        image = NULL;
    }
    */
    Result res = NEWS_AddNotification(title, titleSize, text, textSize, image, imgSize, true);
    consoleSelect(&top);
    if (res) printf("\x1b[1;0HFailed! Press any key to continue.");
    else {
        printf("\x1b[1;0HDone! Press any key to continue.");
        /*NewsList* voice = createVoice(total);
        list->next = voice;
        list = list->next;
        total++;
        */
    }
    waitKey();
}

void printNews(NewsList* list) {
    consoleSelect(&bot);
    consoleClear();
    printf("\x1b[0;0HNOTIFICATION LIST");
    printf("\x1b[%lu;0H>", 1 + selected);
	u32 i = 0;
	while (i < scroll) {
		list = list->next;
		i++;
	}
	i = 0;
	while (list != NULL) {
		if (i > 28) break;
        //if ( (i == 0) && (scroll>0) ) printf("\x1b[37m...");
		if (list->unread) printf("\x1b[%lu;2H\x1b[33m%s [!]\x1b[0m", 1 + i, list->title);
        else printf("\x1b[%lu;2H%s", 1 + i, list->title);
		list = list->next;
		i++;
	}
}

u32 selectNews(NewsList* news) {
    selected = 0;
    scroll = 0;
    printNews(news);
    while ( aptMainLoop() )
    {
        //scan input
        hidScanInput();
        u32 kDown = hidKeysDown();
        
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
        
        //get selected news
        if (kDown & KEY_A) break;
        
        //flush and swap buffers
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    return selected + scroll;
}

void readNews(u32 id, NewsList* list) {
    consoleSelect(&top);
    consoleClear();
    NotificationHeader header;
    NEWS_GetNotificationHeader(id, &header);
    header.unread=false;
    NEWS_SetNotificationHeader(id, (const NotificationHeader*)&header);
    while (list != NULL) {
        NEWS_GetNotificationHeader(list->id, &header);
        list->unread = header.unread;
		list = list->next;
	}
	u16 tmp[0x1780];
	NEWS_GetNotificationMessage(id, tmp);
	char message[0x1780 * 2];
	utf2ascii(message, tmp);
	printf("\x1b[1;0H%s", message);
    printf("\x1b[0;0H\x1b[47;30mNotifyMii v1.0.1                                  \x1b[0m");
    printf("\x1b[29;0H\x1b[47;30mA: Read / X: Delete / B: Back                     \x1b[0m");
    consoleSelect(&bot);
}

void dumpNews (u32 id, NewsList* news) {
	u32 i = 0;
	while (i < id) {
		news = news->next;
		i++;
	}
    
    //generating filenames
	u64 time = (osGetTime() / 1000) % 86400;
	u8 hours = time / 3600;
	u8 minutes = (time % 3600) / 60;
	u8 seconds = time % 60;
	char filename[64];
	char filename2[64];
	sprintf(filename, "/NotifyMii/%u%u%u_%s.txt", hours, minutes, seconds, news->title);
	sprintf(filename2,"/NotifyMii/%u%u%u_%s.jpg", hours, minutes, seconds, news->title);
	
	//getting message
	u16 tmp[0x1780];
	NEWS_GetNotificationMessage(news->id, tmp);
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
	if (news->hasImage) {
		u32 size;
		Handle fileHandle2;
		u8* buffer = (u8*)malloc(0x20000);
		Result ret = NEWS_GetNotificationImage(news->id, buffer, &size);
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

NewsList* deleteNews(u32 id, NewsList* news) {
	NewsList* ret = news;
	NewsList* tmp = news;
	if (id > 0) {
		u32 i = 1;
		while (i < id) {
			tmp = tmp->next;
			i++;
		}
		NewsList* tmp2 = tmp;
		tmp = tmp->next;
		tmp2->next = tmp->next;
	}
    else ret = ret->next;
	NotificationHeader header = { 0 };
	NEWS_SetNotificationHeader(tmp->id, (const NotificationHeader*)&header);
    total--;
    if (scroll>0) scroll--;
    else if (selected >= total) {
        if (total>0) selected=total-1;
    }
	free(tmp);
	return ret;
}

void clearNews(NewsList* list) {
    int errors = 0;
	NotificationHeader header = { 0 };
	while (list != NULL) {
		Result ret = NEWS_SetNotificationHeader(list->id, (const NotificationHeader*)&header);
		if (ret) errors++;
		list = list->next;
	}
	printf("%li notifications deleted!\nPress any key to continue.", total - errors);
    total -= (total - errors);
    waitKey();
}