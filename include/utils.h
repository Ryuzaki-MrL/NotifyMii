#ifndef UTILS_H
#define UTILS_H

extern PrintConsole top;
extern PrintConsole bot;
extern u8 menu;
extern u32 selected;
extern u32 scroll;
extern u32 total;
extern char title[32];
extern char message[0x1780];
extern u32 offset;
extern u8 phase;
extern bool shifted;

void ascii2utf(u16* dst, char* src);
void utf2ascii(char* dst, u16* src);
void waitKey();

#endif /* UTILS_H */
