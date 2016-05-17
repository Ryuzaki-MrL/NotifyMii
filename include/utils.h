#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

extern PrintConsole top;
extern PrintConsole bot;

typedef struct {
    std::string name;
    bool isDir;
} entry;

std::vector<entry> getFileList(std::string directory, std::string extension);
void sortFileList(std::vector<entry> *filelist, std::string curdir);
bool isDirectory(std::string path);
void ascii2utf(u16* dst, char* src);
void utf2ascii(char* dst, u16* src);
void waitKey(void);

#endif /* UTILS_H */
