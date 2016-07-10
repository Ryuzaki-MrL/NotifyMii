#ifndef UI_H
#define UI_H

#include <vector>
#include <string>

enum {
    MODE_SELECTION,
    MODE_NEWS_LIST,
    MODE_FILE_LIST,
    MODE_KEYBOARD_LEGACY,
    MODE_KEYBOARD_TOUCH,
    MODE_NONE
};

void printInfo(u8 mode, bool clear = false);
void printFiles(u32 selected, u32 scroll, u32 count, std::vector<entry>* files, std::string curdir);
void printTitles(u32 selected, u32 scroll, u32 count, std::vector<title_entry>* titles, FS_MediaType media);
void printNews(u32 selected, u32 scroll, bool info = false);
void drawImage(u8* image, u32 imgSize);
void gfxEndFrame();

#endif /* UI_H */
