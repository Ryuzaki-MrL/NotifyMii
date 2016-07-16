#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <3ds.h>

#include "utils.h"

std::vector<entry> getFileList(std::string directory, std::string extension) {
    std::vector<entry> result;
    DIR* dir = opendir(directory.c_str());
    if(dir == NULL) return result;
    dirent* ent = NULL;
    do {
        ent = readdir(dir);
        if (ent != NULL) {
            std::string file(ent->d_name);
            bool isDir = isDirectory(directory + file + "/");
            std::string::size_type dotPos = file.rfind('.');
            if((extension == file.substr(dotPos+1)) || ( isDir ) || (extension == "")) result.push_back({file, isDir});
        }
    } while(ent != NULL);
    closedir(dir);
    sortFileList(&result);
    return result;
}

void sortFileList(std::vector<entry>* filelist) {
    struct alphabetically {
        inline bool operator() (entry a, entry b) {
            if(a.isDir == b.isDir)
                return strcasecmp(a.name.c_str(), b.name.c_str()) < 0;
            else return a.isDir;
        }
    } sort_a;
    std::sort((*filelist).begin(), (*filelist).end(), sort_a);
}

bool isDirectory(std::string path) {
    bool result = false;
    DIR *dir = opendir(path.c_str());
    if (dir != NULL) result = true;
    closedir(dir);
    return result;
}

// https://github.com/Rinnegatamante/lpp-3ds/blob/master/source/include/utils.cpp#L76-L80
void ascii2utf(u16* dst, char* src) {
    if (!src || !dst) return;
    while(*src) *(dst++)=(*src++);
    *dst=0x00;
}

// https://github.com/Rinnegatamante/lpp-3ds/blob/master/source/include/utils.cpp#L70-L74
void utf2ascii(char* dst, u16* src) {
    if (!src || !dst) return;
    while(*src) *(dst++)=(*(src++))&0xFF;
    *dst=0x00;
}

std::string validateFileName(std::string fname) {
    fname.erase(std::remove(fname.begin(), fname.end(), ':'), fname.end());
    fname.erase(std::remove(fname.begin(), fname.end(), '?'), fname.end());
    fname.erase(std::remove(fname.begin(), fname.end(), '/'), fname.end());
    fname.erase(std::remove(fname.begin(), fname.end(), '\\'), fname.end());
    fname.erase(std::remove(fname.begin(), fname.end(), '*'), fname.end());
    fname.erase(std::remove(fname.begin(), fname.end(), '<'), fname.end());
    fname.erase(std::remove(fname.begin(), fname.end(), '>'), fname.end());
    fname.erase(std::remove(fname.begin(), fname.end(), '|'), fname.end());
    return fname;
}

void waitKey() {
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown) break;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}
