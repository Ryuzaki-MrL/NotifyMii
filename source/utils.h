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

typedef struct {
    u64 id;
    std::string name;
} title_entry;

typedef struct {
    u16 shortDescription[0x40];
    u16 longDescription[0x80];
    u16 publisher[0x40];
} SMDH_title;

typedef struct {
    char magic[0x04];
    u16 version;
    u16 reserved1;
    SMDH_title titles[0x10];
    u8 ratings[0x10];
    u32 region;
    u32 matchMakerId;
    u64 matchMakerBitId;
    u32 flags;
    u16 eulaVersion;
    u16 reserved;
    u32 optimalBannerFrame;
    u32 streetpassId;
    u64 reserved2;
    u8 smallIcon[0x480];
    u8 largeIcon[0x1200];
} SMDH;

std::vector<entry> getFileList(std::string directory, std::string extension);
void sortFileList(std::vector<entry> *filelist);
bool isDirectory(std::string path);
void ascii2utf(u16* dst, char* src);
void utf2ascii(char* dst, u16* src);
void waitKey();

#endif /* UTILS_H */
