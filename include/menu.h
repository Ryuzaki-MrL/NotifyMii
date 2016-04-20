#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>

enum {
    MENU_MAIN,
    MENU_ADD_TITLE,
    MENU_LIST,
    MENU_CLEAR,
    MENU_EXIT,
    MENU_ADD_MESSAGE_SELECT,
    MENU_ADD_MESSAGE_KEYBOARD,
    MENU_ADD_MESSAGE_FILE,
    MENU_ADD_MESSAGE_NEWS,
    MENU_ADD_IMAGE_SELECT,
    MENU_ADD_IMAGE_DEFAULT,
    MENU_ADD_IMAGE_FILE,
    MENU_ADD_IMAGE_NEWS,
    MENU_ADD_IMAGE_EMPTY,
    MENU_ADD_PROCESS_SELECT,
    MENU_ADD_PROCESS_SELF,
    MENU_ADD_PROCESS_BROWSE,
    MENU_ADD_PROCESS_NEWS,
    MENU_ADD_NEWS
};

void printFiles(u32 selected, u32 scroll, u32 count, std::vector<std::string> *files, std::string curdir);
void printTitles(u32 selected, u32 scroll, u32 count, std::vector<u64> *titles, u8 media);
std::vector<std::string> getFileList(std::string directory, std::string extension);
bool isDirectory(std::string path);

void menuMain(u8 *menu);
void menuAddTitle(u8 *menu, char *title);
void menuList(u8 *menu);
void menuClear(u8 *menu);
void menuAddMessageSelect(u8 *menu);
void menuAddMessageKeyboard(u8 *menu, char *message);
void menuAddMessageFile(u8 *menu, char *message);
void menuAddMessageNews(u8 *menu, char *message);
void menuAddImageSelect(u8 *menu);
void menuAddImageDefault(u8 *menu, u8 *image, u32 *imgSize);
void menuAddImageFile(u8 *menu, u8 *image, u32 *imgSize);
void menuAddImageNews(u8 *menu, u8 *image, u32 *imgSize);
void menuAddImageEmpty(u8 *menu, u8 *image, u32 *imgSize, bool *hasImage);
void menuAddProcessSelect(u8 *menu, u64 *processID);
void menuAddProcessSelf(u8 *menu, u64 *processID);
void menuAddProcessBrowse(u8 *menu, u64 *processID);
void menuAddProcessNews(u8 *menu, u64 *processID);
void menuAddNews(u8 *menu, char *title_c, char *message_c, u8 *image, u32 *imgSize, bool *hasImage, u64 *processID);

#endif /* MENU_H */
