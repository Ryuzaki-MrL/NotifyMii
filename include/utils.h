#ifndef UTILS_H
#define UTILS_H

extern PrintConsole top;
extern PrintConsole bot;

void ascii2utf(u16* dst, char* src);
void utf2ascii(char* dst, u16* src);
void waitKey();

#endif /* UTILS_H */
