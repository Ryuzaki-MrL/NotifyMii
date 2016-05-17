#include <stdio.h>
#include <vector>
#include <string>
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
            if( (extension == file.substr(dotPos+1)) || ( isDir ) ) result.push_back({file, isDir});
        }
    } while(ent != NULL);
    
    closedir(dir);
    sortFileList(&result);
    return result;
}


void sortFileList(std::vector<entry> *filelist) {
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

//Thanks Rinnegatamante
void ascii2utf(u16* dst, char* src) {
    if (!src || !dst) return;
    while(*src) *(dst++)=(*src++);
    *dst=0x00;
}

//Thanks Rinnegatamante
void utf2ascii(char* dst, u16* src) {
    if (!src || !dst) return;
    while(*src) *(dst++)=(*(src++))&0xFF;
    *dst=0x00;
}


void waitKey(void) {
    while ( aptMainLoop() ) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown) break;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}