#ifndef MENU_H
#define MENU_H

enum {
    MENU_MAIN,
    MENU_NEWS_ADD_TITLE,
    MENU_NEWS_LIST,
    MENU_UPDATE,
    MENU_EXIT,
    MENU_NEWS_ADD_MESSAGE,
    MENU_NEWS_ADD_IMAGE,
    MENU_NEWS_ADD_PROCESS,
    MENU_NEWS_ADD_FLAGS,
    MENU_NEWS_ADD
};

u8 selectionMenu(std::string text, u8 entries);
u8 getNotificationID();
u64 getTitleID();
std::string getFileName(std::string filter = "");

void menuMain(u8* menu);
void menuNewsAddTitle(u8* menu, char* title);
void menuNewsList(u8* menu);
void menuUpdate(u8* menu);
void menuNewsAddMessage(u8* menu, char* message);
void menuNewsAddImage(u8* menu, u8* image, u32* imgSize);
void menuNewsAddProcess(u8* menu, u64* processID);
void menuNewsAddFlags(u8* menu);
void menuAddNews(u8* menu, char* title_c, char* message_c, u8* image, u32 imgSize, u64 processID, news_flags flags = { true, false, false });

#endif /* MENU_H */
