#ifndef UI_H
#define UI_H

#include <vector>
#include <string>

void printInfo(u8 mode);
void printFiles(u32 selected, u32 scroll, u32 count, std::vector<entry> *files, std::string curdir);
void printTitles(u32 selected, u32 scroll, u32 count, std::vector<u64> *titles, FS_MediaType media);
void drawImage(u8 *image, u32 imgSize);
void gfxEndFrame(void);

#endif /* UI_H */
